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
package org.sipfoundry.sipxconfig.site.phone;

import java.util.ArrayList;
import java.util.List;

import org.apache.tapestry.IRequestCycle;
import org.apache.tapestry.event.PageEvent;
import org.apache.tapestry.event.PageRenderListener;
import org.apache.tapestry.html.BasePage;
import org.sipfoundry.sipxconfig.components.SelectMap;
import org.sipfoundry.sipxconfig.components.TapestryUtils;
import org.sipfoundry.sipxconfig.phone.Line;
import org.sipfoundry.sipxconfig.phone.Phone;
import org.sipfoundry.sipxconfig.phone.PhoneContext;
import org.sipfoundry.sipxconfig.site.line.NewLine;


/**
 * Comments
 */
public abstract class PhoneLines extends BasePage implements PageRenderListener {

    public static final String PAGE = "PhoneLines";

    public abstract Phone getPhone();
    
    public abstract void setPhone(Phone phone);
    
    public abstract int getPhoneId();
    
    /** REQUIRED PROPERTY */
    public abstract void setPhoneId(int id);

    public abstract List getLines();
    
    public abstract void setLines(List lines);
    
    public abstract Line getCurrentRow();

    public abstract void setCurrentRow(Line line);
    
    public abstract SelectMap getSelections();
    
    public abstract void setSelections(SelectMap selections);
    
    public void pageBeginRender(PageEvent event) {
        PhoneContext context = PhonePageUtils.getPhoneContext(event.getRequestCycle());
        setPhone(context.getPhone(getPhoneId()));

        setLines(new ArrayList());
        
        // Generate the list of phone items
        if (getSelections() == null) {
            setSelections(new SelectMap());
        }
    }
    
    public void newLine(IRequestCycle cycle) {
        PhoneContext context = PhonePageUtils.getPhoneContext(cycle);
        Object[] params = cycle.getServiceParameters();
        Integer endpointId = (Integer) TapestryUtils.assertParameter(Integer.class, params, 0);
        Phone phone = context.getPhone(endpointId.intValue());
        NewLine page = (NewLine) cycle.getPage(NewLine.PAGE);
        page.setPhone(phone);
        page.setReturnPage(PAGE);
        cycle.activate(page);
    }
}
