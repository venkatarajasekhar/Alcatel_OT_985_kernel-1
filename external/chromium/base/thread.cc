// Copyright (c) 2006-2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/thread.h"

#include "base/dynamic_annotations.h"
#include "base/lazy_instance.h"
#include "base/string_util.h"
#include "base/thread_local.h"
#include "base/waitable_event.h"

namespace base {

// This task is used to trigger the message loop to exit.
class ThreadQuitTask : public Task {
 public:
  virtual void Run() {
    MessageLoop::current()->Quit();
    Thread::SetThreadWasQuitProperly(true);
  }
};

// Used to pass data to ThreadMain.  This structure is allocated on the stack
// from within StartWithOptions.
struct Thread::StartupData {
  // We get away with a const reference here because of how we are allocated.
  const Thread::Options& options;

  // Used to synchronize thread startup.
  WaitableEvent event;

  explicit StartupData(const Options& opt)
      : options(opt),
        event(false, false) {}
};

Thread::Thread(const char *name)
    : stopping_(false),
      startup_data_(NULL),
      thread_(0),
      message_loop_(NULL),
      thread_id_(0),
      name_(name) {
}

Thread::~Thread() {
  Stop();
}

namespace {

// We use this thread-local variable to record whether or not a thread exited
// because its Stop method was called.  This allows us to catch cases where
// MessageLoop::Quit() is called directly, which is unexpected when using a
// Thread to setup and run a MessageLoop.
base::LazyInstance<base::ThreadLocalBoolean> lazy_tls_bool(
    base::LINKER_INITIALIZED);

}  // namespace

void Thread::SetThreadWasQuitProperly(bool flag) {
  lazy_tls_bool.Pointer()->Set(flag);
}

bool Thread::GetThreadWasQuitProperly() {
  bool quit_properly = true;
#ifndef NDEBUG
  quit_properly = lazy_tls_bool.Pointer()->Get();
#endif
  return quit_properly;
}

bool Thread::Start() {
  return StartWithOptions(Options());
}

bool Thread::StartWithOptions(const Options& options) {
  DCHECK(!message_loop_);

  SetThreadWasQuitProperly(false);

  StartupData startup_data(options);
  startup_data_ = &startup_data;

  if (!PlatformThread::Create(options.stack_size, this, &thread_)) {
    DLOG(ERROR) << "failed to create thread";
    startup_data_ = NULL;  // Record that we failed to start.
    return false;
  }

  // Wait for the thread to start and initialize message_loop_
  startup_data.event.Wait();

  DCHECK(message_loop_);
  return true;
}

void Thread::Stop() {
  if (!thread_was_started())
    return;

  StopSoon();

  // Wait for the thread to exit.
  //
  // TODO(darin): Unfortunately, we need to keep message_loop_ around until
  // the thread exits.  Some consumers are abusing the API.  Make them stop.
  //
  PlatformThread::Join(thread_);

  // The thread should NULL message_loop_ on exit.
  DCHECK(!message_loop_);

  // The thread no longer needs to be joined.
  startup_data_ = NULL;

  stopping_ = false;
}

void Thread::StopSoon() {
  // We should only be called on the same thread that started us.
  DCHECK_NE(thread_id_, PlatformThread::CurrentId());

  if (stopping_ || !message_loop_)
    return;

  stopping_ = true;
  message_loop_->PostTask(FROM_HERE, new ThreadQuitTask());
}

void Thread::Run(MessageLoop* message_loop) {
  message_loop->Run();
}

void Thread::ThreadMain() {
  // The message loop for this thread.
  MessageLoop message_loop(startup_data_->options.message_loop_type);

  // Complete the initialization of our Thread object.
  thread_id_ = PlatformThread::CurrentId();
  PlatformThread::SetName(name_.c_str());
  ANNOTATE_THREAD_NAME(name_.c_str());  // Tell the name to race detector.
  message_loop.set_thread_name(name_);
  message_loop_ = &message_loop;

  // Let the thread do extra initialization.
  // Let's do this before signaling we are started.
  Init();

  startup_data_->event.Signal();
  // startup_data_ can't be touched anymore since the starting thread is now
  // unlocked.

  Run(message_loop_);

  // Let the thread do extra cleanup.
  CleanUp();

  // Assert that MessageLoop::Quit was called by ThreadQuitTask.
  DCHECK(GetThreadWasQuitProperly());

  // We can't receive messages anymore.
  message_loop_ = NULL;
  thread_id_ = 0;
}

}  // namespace base
