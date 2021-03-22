#include "common/common/logger_delegates.h"

#include <cassert> // use direct system-assert to avoid cyclic dependency.
#include <cstdint>
#include <iostream>
#include <string>

#include "spdlog/spdlog.h"

namespace Envoy {
namespace Logger {
FileSinkDelegate::FileSinkDelegate(const std::string& log_path,
                                   AccessLog::AccessLogManager& log_manager,
                                   DelegatingLogSinkSharedPtr log_sink)
    : SinkDelegate(log_sink), log_file_(log_manager.createAccessLog(Filesystem::FilePathAndType{
                                  Filesystem::DestinationType::File, log_path})) {
  setDelegate();
}

FileSinkDelegate::~FileSinkDelegate() { restoreDelegate(); }

void FileSinkDelegate::log(absl::string_view msg) {
  // Log files have internal locking to ensure serial, non-interleaved
  // writes, so no additional locking needed here.
  log_file_->write(msg);
}

void FileSinkDelegate::flush() {
  // Log files have internal locking to ensure serial, non-interleaved
  // writes, so no additional locking needed here.
  log_file_->flush();
}

SplicingSinkDelegate::SplicingSinkDelegate(std::unique_ptr<SinkDelegate> sink_a,
                                           std::unique_ptr<SinkDelegate> sink_b,
                                           DelegatingLogSinkSharedPtr log_sink)
    : SinkDelegate(log_sink), sink_a_(std::move(sink_a)), sink_b_(std::move(sink_b)) {
  setDelegate();
}

SplicingSinkDelegate::~SplicingSinkDelegate() { restoreDelegate(); }

void SplicingSinkDelegate::log(absl::string_view msg) {
  sink_a_->log(msg);
  sink_b_->log(msg);
}

void SplicingSinkDelegate::flush() {
  sink_a_->flush();
  sink_b_->flush();
}

TestSinkDelegate::TestSinkDelegate(DelegatingLogSinkSharedPtr log_sink)
    : SinkDelegate(log_sink) {
  setDelegate();
}

TestSinkDelegate::~TestSinkDelegate() { restoreDelegate(); }

void TestSinkDelegate::log(absl::string_view msg) {
  std::cerr << "#" << msg;
}

void TestSinkDelegate::flush() {
}


} // namespace Logger
} // namespace Envoy
