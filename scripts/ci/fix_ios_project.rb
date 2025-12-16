#!/usr/bin/env ruby
require "xcodeproj"

proj = Xcodeproj::Project.open("ios/JunoNative.xcodeproj")
target = proj.targets.first

target.product_type = "com.apple.product-type.application"
target.product_reference.path = "JunoNative.app"
target.product_reference.explicit_file_type = "wrapper.application"

target.build_configurations.each do |c|
  c.build_settings["INFOPLIST_FILE"] = "JunoNative/Info.plist"
  c.build_settings["SKIP_INSTALL"] = "NO"
  c.build_settings["LD_RUNPATH_SEARCH_PATHS"] = [
    "$(inherited)",
    "@executable_path/Frameworks"
  ]
end

proj.save
puts "✅ Converted static library to iOS Application"
