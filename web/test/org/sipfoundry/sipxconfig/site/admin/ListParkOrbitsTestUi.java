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
package org.sipfoundry.sipxconfig.site.admin;

import junit.framework.Test;

import org.sipfoundry.sipxconfig.site.ListWebTestCase;
import org.sipfoundry.sipxconfig.site.SiteTestHelper;
import org.sipfoundry.sipxconfig.site.dialplan.EditAutoAttendantTestUi;

public class ListParkOrbitsTestUi extends ListWebTestCase {
    private String m_musicOnHoldFileName;

    public static Test suite() throws Exception {
        return SiteTestHelper.webTestSuite(ListParkOrbitsTestUi.class);
    }

    public ListParkOrbitsTestUi() throws Exception {
        super("ListParkOrbits", "resetCallGroupContext", "orbits");
        setHasDuplicate(false);
        m_musicOnHoldFileName = EditAutoAttendantTestUi.seedPromptFile("parkserver/music");
    }

    public void testDisplay() {
        super.testDisplay();
        assertButtonPresent(buildId("activate"));
    }

    protected String[] getParamNames() {
        return new String[] {
            "name", "extension", "description",
        };
    }

    protected String[] getParamValues(int i) {
        return new String[] {
            "orbit" + i, Integer.toString(127 + i), "orbit description + i",
        };
    }

    protected Object[] getExpectedTableRow(String[] paramValues) {
        return new Object[] {
            paramValues[0], "false", paramValues[1], m_musicOnHoldFileName
        };
    }

    protected void setAddParams(String[] names, String[] values) {
        super.setAddParams(names, values);
        tester.selectOption("prompt", m_musicOnHoldFileName);
    }
}
