#!/usr/bin/env ruby
# frozen_string_literal: true

require "fileutils"
require "open3"

ROOT_DIR = File.expand_path("../..", __dir__)
IOS_DIR  = File.join(ROOT_DIR, "ios")

spec_path    = File.join(IOS_DIR, "JunoNativeApp.yml")
output_proj  = File.join(IOS_DIR, "JunoNativeApp.xcodeproj")

unless File.exist?(spec_path)
  abort "❌ Missing XcodeGen spec file: #{spec_path}"
end

def run!(cmd, cwd: nil)
  puts "→ #{cmd}"
  stdout, stderr, status = Open3.capture3(cmd, chdir: cwd)
  unless status.success?
    warn stdout unless stdout.empty?
    warn stderr unless stderr.empty?
    abort "❌ Command failed (exit #{status.exitstatus}): #{cmd}"
  end
  puts stdout unless stdout.empty?
end

# Verify XcodeGen exists.
xcodegen = `which xcodegen 2>/dev/null`.strip
if xcodegen.empty?
  abort <<~MSG
    ❌ XcodeGen not found in PATH.

    On GitHub Actions (macos-* runner), add a step:
      brew install xcodegen

    Locally (macOS), install:
      brew install xcodegen
  MSG
end

# Print version (helps debugging on CI logs)
run!("xcodegen version")

# (Re)generate project deterministically.
FileUtils.rm_rf(output_proj) if Dir.exist?(output_proj)

# Use the ios/ directory as working dir so relative paths inside the spec behave.
run!("xcodegen generate --spec #{File.basename(spec_path)}", cwd: IOS_DIR)

unless Dir.exist?(output_proj)
  abort "❌ Expected generated project not found: #{output_proj}"
end

puts "✅ Generated: #{output_proj}"
