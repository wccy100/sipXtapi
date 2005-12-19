/*
 * 
 * 
 * Copyright (C) 2005 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2005 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.admin.dialplan.attendant;

import java.util.Calendar;
import java.util.TimeZone;

import junit.framework.TestCase;

import org.sipfoundry.sipxconfig.admin.dialplan.attendant.WorkingTime.WorkingHours;

public class WorkingTimeTest extends TestCase {

    protected void setUp() throws Exception {
        super.setUp();
    }

    public void testGetStartTime() throws Exception {
        Calendar c = Calendar.getInstance(TimeZone.getTimeZone("GMT"));
        c.set(1970, Calendar.JANUARY, 1, 10, 14);
        WorkingHours hours = new WorkingHours();
        hours.setStart(c.getTime());
        assertEquals("10:14", hours.getStartTime());
        assertEquals(c.getTime(), hours.getStart());
    }

    public void testGetStopTime() throws Exception {
        Calendar c = Calendar.getInstance(TimeZone.getTimeZone("GMT"));
        c.set(1970, Calendar.JANUARY, 1);
        c.set(Calendar.HOUR, 6);
        c.set(Calendar.MINUTE, 16);
        c.set(Calendar.AM_PM, Calendar.PM);
        WorkingHours hours = new WorkingHours();
        hours.setStop(c.getTime());
        assertEquals("18:16", hours.getStopTime());
        assertEquals(c.getTime(), hours.getStop());
    }

    public void testInitWokingHours() {
        WorkingHours hours = new WorkingHours();
        assertEquals("09:00", hours.getStartTime());
        assertEquals("18:00", hours.getStopTime());
    }

    public void testInitWorkingTime() {
        WorkingTime wt = new WorkingTime();
        WorkingHours[] workingHours = wt.getWorkingHours();
        assertEquals(7, workingHours.length);
        assertTrue(workingHours[0].isEnabled());
        assertTrue(workingHours[4].isEnabled());
        assertFalse(workingHours[5].isEnabled());
        assertFalse(workingHours[6].isEnabled());
    }
}
