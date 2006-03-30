#
# Copyright (C) 2006 SIPfoundry Inc.
# Licensed by SIPfoundry under the LGPL license.
# 
# Copyright (C) 2006 Pingtel Corp.
# Licensed to SIPfoundry under a Contributor Agreement.
#
##############################################################################

require File.join(File.dirname(__FILE__), '..', 'test_helper')


class CdrTest < Test::Unit::TestCase
  fixtures :parties, :cdrs

  def test_load_cdrs
    assert_kind_of Cdr, cdrs(:first)
  end
  
  def test_complete?
    cdr = Cdr.new
    
    # No termination code => CDR is not complete
    assert(!cdr.complete?)
    
    # Termination code of completion or failure implies complete CDR
    cdr.termination = Cdr::CALL_COMPLETED_TERM
    assert(cdr.complete?)
    cdr.termination = Cdr::CALL_FAILED_TERM
    assert(cdr.complete?)
    
    # Termination code of request or in progress implies incomplete CDR
    cdr.termination = Cdr::CALL_REQUESTED_TERM
    assert(!cdr.complete?)
    cdr.termination = Cdr::CALL_IN_PROGRESS_TERM
    assert(!cdr.complete?)
  end
  
end
