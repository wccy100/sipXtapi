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
package org.sipfoundry.sipxconfig.site;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.tapestry.IRequestCycle;
import org.apache.tapestry.engine.BaseEngine;
import org.apache.tapestry.request.RequestContext;
import org.apache.tapestry.request.ResponseOutputStream;
import org.springframework.web.context.WebApplicationContext;
import org.springframework.web.context.support.WebApplicationContextUtils;

/**
 * Hook Spring into Tapestry's global application context
 */
public class SipxconfigEngine extends BaseEngine {
    private static final Log LOG = LogFactory.getLog(SipxconfigEngine.class);
    private static final long serialVersionUID;

    static {
        serialVersionUID = 1L;
    }

    /**
     * Inserts application context in global.
     * 
     * In ognl use: <code>global.beanName</code> to get Spring beans. You can also use
     * <code>global.sipXconfigContext.getBean("beanName")</code>
     * 
     */
    protected Object createGlobal(RequestContext context) {
        BeanFactoryGlobals global = (BeanFactoryGlobals) super.createGlobal(context);
        ServletContext servletContext = context.getServlet().getServletContext();
        WebApplicationContext bf = WebApplicationContextUtils
                .getWebApplicationContext(servletContext);
        global.setApplicationContext(bf);
        return global;
    }

    /**
     * Create Visit object from Spring application context which allows us to configure Visit
     * using bean configuration files.
     * 
     * Setting "org.apache.tapestry.visit-class" property in .application file does not have any
     * effect.
     */
    protected Object createVisit(IRequestCycle cycle_) {
        BeanFactoryGlobals global = (BeanFactoryGlobals) getGlobal();
        return global.getApplicationContext().getBean("visit", Visit.class);
    }

    /**
     * Call super implementation but log exception first. Default implementation only logs it the
     * are problems on exception page
     */
    protected void activateExceptionPage(IRequestCycle cycle, ResponseOutputStream output,
            Throwable cause) throws ServletException {
        LOG.warn("Tapestry exception", cause);
        super.activateExceptionPage(cycle, output, cause);
    }

    /**
     * To display standard "developers" error page call super.getExceptionPage instead
     */
    protected String getExceptionPageName() {
        return "InternalErrorPage";
    }
}
