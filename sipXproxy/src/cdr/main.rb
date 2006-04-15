#!/usr/bin/env ruby
#
# Copyright (C) 2006 SIPfoundry Inc.
# Licensed by SIPfoundry under the LGPL license.
# 
# Copyright (C) 2006 Pingtel Corp.
# Licensed to SIPfoundry under a Contributor Agreement.
#
##############################################################################

# system requires
require 'getoptlong'
require 'parsedate'

# application requires
require File.join(File.dirname(__FILE__), 'call_resolver')
require 'call_direction_plugin'


# Create a Call Resolver
resolver = CallResolver.new

if resolver.log.debug?
  resolver.log.debug("main called with these args: #{ARGV.join(' ')}")
end

# Parse command-line options
#   start: date/time from which to start analyzing call events
#   end:   date/time at which to end the analysis (defaults to 1 day later)
#   redo:  if true then calculate all CDRs, even those already done before
#          (defaults to false)
#   daily: perform daily processing based on the configuration -- resolve
#          calls and/or purge old data
# :TODO: Print out a helpful message if the caller enters no options or screws up
opts = GetoptLong.new(
  [ "--start", "-s", GetoptLong::OPTIONAL_ARGUMENT ],
  [ "--end",   "-e", GetoptLong::OPTIONAL_ARGUMENT ],
  [ "--redo",  "-r", GetoptLong::NO_ARGUMENT ],
  [ "--daily", "-d", GetoptLong::NO_ARGUMENT ],
  [ "--help",  "-h", GetoptLong::NO_ARGUMENT ]
)

# Init options
start_time = end_time = nil
redo_flag = false
daily_flag = false

# Extract option values
# Convert start and end strings to date/time values.
opts.each do |opt, arg|
  case opt

  when "--start"
    start_time = Time.parse(arg)

  when "--end"
    end_time = Time.parse(arg)

  when "--redo"
    redo_flag = true

  when "--daily"
    daily_flag = true
 
  when "--help"
    print <<EOT
usage: sipxcallresolver.sh [--start "time" [--end "time"]] [--daily] [--help]

  --start "time"  Specifies the start time of the window in which calls are to 
                  be resolved. The time format is ISO 8601.
  --end "time"    Specifies the end time of the window in which call are to be
                  resolved. The time format is ISO 8601.
  --daily         Indicates that the call resolver is called in 'daily' mode:
                  * Create CDRs for calls in the previous 24 hours
                  * Purge CSE and CDR data older than 35 days
                  You can change the purge age by setting the configuration
                  parameter SIP_CALLRESOLVER_PURGE_AGE (units are days).
EOT

  end
end 

# Add the Call Direction Plugin as an observer so that it can compute call direction
if CallDirectionPlugin.call_direction?(resolver.config)
  resolver.log.info("Call direction is enabled")
  resolver.add_observer(CallDirectionPlugin.new(resolver))
end

# Resolve calls that occurred during the specified time window
resolver.resolve(start_time, end_time, redo_flag, daily_flag)
