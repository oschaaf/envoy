#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "envoy/access_log/access_log.h"

#include "common/common/logger.h"
#include "common/common/macros.h"

#include "absl/strings/string_view.h"

namespace Envoy {
namespace Logger {

/**
 * SinkDelegate that writes log messages to a file.
 */
class FileSinkDelegate : public SinkDelegate {
public:
  FileSinkDelegate(const std::string& log_path, AccessLog::AccessLogManager& log_manager,
                   DelegatingLogSinkSharedPtr log_sink);
  ~FileSinkDelegate() override;

  // SinkDelegate
  void log(absl::string_view msg) override;
  void flush() override;

private:
  AccessLog::AccessLogFileSharedPtr log_file_;
};

class SplicingSinkDelegate : public SinkDelegate {
public:
  SplicingSinkDelegate(std::unique_ptr<SinkDelegate> sink_a,
                       std::unique_ptr<SinkDelegate> sink_b, DelegatingLogSinkSharedPtr log_sink);
  ~SplicingSinkDelegate() override;

  // SinkDelegate
  void log(absl::string_view msg) override;
  void flush() override;

private:
  std::unique_ptr<SinkDelegate> sink_a_;
  std::unique_ptr<SinkDelegate> sink_b_;
  DelegatingLogSinkSharedPtr log_sink_;
};

class TestSinkDelegate : public SinkDelegate {
public:
  TestSinkDelegate(DelegatingLogSinkSharedPtr log_sink);
  ~TestSinkDelegate() override;

  // SinkDelegate
  void log(absl::string_view msg) override;
  void flush() override;
};

} // namespace Logger

} // namespace Envoy
