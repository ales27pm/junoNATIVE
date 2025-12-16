#!/usr/bin/env ruby
require "xcodeproj"

project_path = Dir.glob("ios/*.xcodeproj").first
abort("❌ No xcodeproj found") unless project_path

project = Xcodeproj::Project.open(project_path)

project.targets.each do |target|
  next unless target.product_type == "com.apple.product-type.application"

  target.build_configurations.each do |config|
    config.build_settings["SKIP_INSTALL"] = "NO"
    config.build_settings["INSTALL_PATH"] = "$(LOCAL_APPS_DIR)"
    config.build_settings["CODE_SIGNING_ALLOWED"] = "YES"
    config.build_settings["BUILD_LIBRARY_FOR_DISTRIBUTION"] = "NO"
  end

  puts "✅ Fixed settings for app target: #{target.name}"
end

project.save
