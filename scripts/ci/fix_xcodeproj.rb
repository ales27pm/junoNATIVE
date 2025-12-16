#!/usr/bin/env ruby
# scripts/ci/fix_xcodeproj.rb
#
# Fixes the #1 cause of "generic archive" in CI:
# - ensures iOS application targets have SKIP_INSTALL=NO for all configs
#
# This runs on GitHub Actions (macOS runner) via `bundle exec ruby scripts/ci/fix_xcodeproj.rb`.

require "pathname"

ROOT = Pathname.new(__dir__).join("..", "..").cleanpath
XCODEPROJ = ROOT.join("ios", "JunoNative.xcodeproj")
PBXPROJ = XCODEPROJ.join("project.pbxproj")

abort("❌ Missing Xcode project: #{XCODEPROJ}") unless XCODEPROJ.exist?
abort("❌ Missing pbxproj: #{PBXPROJ}") unless PBXPROJ.exist?

begin
  require "xcodeproj"
rescue LoadError => e
  warn "❌ Missing gem 'xcodeproj'. It should be present if you're using CocoaPods via Bundler."
  warn "   Error: #{e.class}: #{e.message}"
  warn "   Fix: ensure Gemfile includes `gem 'cocoapods'` (it pulls in xcodeproj), then run `bundle install`."
  exit 1
end

project = Xcodeproj::Project.open(XCODEPROJ.to_s)

app_targets = project.targets.select do |t|
  t.respond_to?(:product_type) && t.product_type == "com.apple.product-type.application"
end

if app_targets.empty?
  warn "❌ No iOS application targets found in #{XCODEPROJ}."
  warn "   Targets seen: #{project.targets.map(&:name).join(', ')}"
  exit 1
end

changed = false

app_targets.each do |t|
  t.build_configurations.each do |cfg|
    current = cfg.build_settings["SKIP_INSTALL"]
    if current != "NO"
      cfg.build_settings["SKIP_INSTALL"] = "NO"
      changed = true
      puts "✅ Set SKIP_INSTALL=NO for target=#{t.name} config=#{cfg.name} (was #{current.inspect})"
    end
  end
end

if changed
  project.save
  puts "💾 Saved project changes to #{PBXPROJ}"
else
  puts "✅ No changes needed (SKIP_INSTALL already NO on app targets)."
end
