/*
 * 
 * 
 * Copyright (C) 2004 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2004 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.phone;

import java.util.List;

import junit.framework.TestCase;

/**
 * Requires Database access.
 */
public class PhoneDaoImplTestDb extends TestCase {

    private PhoneDao m_dao;

    private PhoneContext m_context;

    private UnitTestDao m_testData;

    protected void setUp() throws Exception {
        super.setUp();

        m_context = PhoneTestHelper.getPhoneContextWithDb();
        assertNotNull(m_context);
        m_dao = m_context.getPhoneDao();
        assertNotNull(m_dao);
        m_testData = PhoneTestHelper.getUnitTestDao();
        assertNotNull(m_testData);
        assertTrue(m_testData.initializeImmutableData());
    }

    protected void tearDown() throws Exception {
        assertTrue(m_testData.verifyDataUnaltered());
    }

    public void testSampleData() {
        assertNotNull(m_testData.createSampleLine());
        assertNotNull(m_testData.createSampleSettingSet());
        assertNotNull(m_testData.createSampleEndpoint());
    }
    
    public void testSampleEndpointLine() {
        assertNotNull(m_testData.createSampleEndpointLine());        
    }

    public void testLoadPhoneSummaries() {
        int preSize = m_dao.loadPhoneSummaries(m_context).size();

        EndpointLine eline = m_testData.createSampleEndpointLine();
        assertNotNull(eline);
        
        // just test there's one more in list, not a very 
        // hard test
        List summaries = m_dao.loadPhoneSummaries(m_context);
        assertEquals(preSize + 1, summaries.size());
    }

    /**
     * PhoneSummaryFactory implementation
     */
    public PhoneSummary createPhoneSummary() {
        return new PhoneSummary();
    }

    /**
     * PhoneSummaryFactory implementation
     */
    public PhoneContext getPhoneContext() {
        return m_context;
    }
}