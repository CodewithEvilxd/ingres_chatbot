// Vercel Serverless Function for INGRES ChatBot API
// This acts as a bridge between the web frontend and the C backend

const { spawn } = require('child_process');
const path = require('path');
const { authenticateRequest, checkRateLimit, validateInput, ROLES } = require('./auth.js');

module.exports = async (req, res) => {
  // Enable CORS
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization');

  if (req.method === 'OPTIONS') {
    res.status(200).end();
    return;
  }

  if (req.method !== 'POST') {
    return res.status(405).json({ error: 'Method not allowed' });
  }

  try {
    // Authenticate request
    const auth = authenticateRequest(req);
    if (!auth.authenticated) {
      return res.status(401).json({
        error: 'Authentication required',
        message: 'Please login to access the chat service'
      });
    }

    const { message, language = 'en' } = req.body;

    // Validate input
    if (!message) {
      return res.status(400).json({ error: 'Message is required' });
    }

    const messageValidation = validateInput(message, 'message');
    if (!messageValidation.valid) {
      return res.status(400).json({ error: messageValidation.error });
    }

    // Check rate limiting (different limits based on user role)
    const maxRequests = auth.user.role === ROLES.ADMIN ? 1000 :
                       auth.user.role === ROLES.ANALYST ? 500 : 100;
    const rateLimit = checkRateLimit(`chat_${auth.user.userId}`, maxRequests, 60 * 60 * 1000); // per hour

    if (!rateLimit.allowed) {
      return res.status(429).json({
        error: 'Rate limit exceeded',
        message: `Too many requests. Try again in ${Math.ceil((rateLimit.resetTime - Date.now()) / 1000 / 60)} minutes`,
        retryAfter: Math.ceil((rateLimit.resetTime - Date.now()) / 1000)
      });
    }

    // For Vercel deployment, we'll use a simulated response
    // In production, this would call the actual C binary
    const response = await processChatMessage(message, language);

    res.status(200).json({
      message: response.message,
      intent: response.intent,
      confidence: response.confidence,
      processing_time_ms: response.processingTime,
      has_data: response.hasData,
      requires_clarification: response.requiresClarification,
      suggestions: response.suggestions || [],
      data_sources: response.dataSources || []
    });

  } catch (error) {
    console.error('API Error:', error);
    res.status(500).json({
      error: 'Internal server error',
      message: 'Sorry, I encountered an error processing your request.'
    });
  }
};

// Simulated chat processing (replace with actual C binary call in production)
async function processChatMessage(message, language) {
  const startTime = Date.now();

  // Convert message to lowercase for processing
  const lowerMessage = message.toLowerCase();

  // Enhanced intent classification simulation
  let intent = 'unknown';
  let confidence = 0.5;
  let responseMessage = '';
  let hasData = false;
  let requiresClarification = false;
  let suggestions = [];
  let dataSources = ['Central Ground Water Board (CGWB)'];

  // Location queries
  if (lowerMessage.includes('punjab') || lowerMessage.includes('data') || lowerMessage.includes('show')) {
    intent = 'location_query';
    confidence = 0.9;
    responseMessage = `📍 **Groundwater Analysis for Punjab**\n\n📊 **Assessment Year**: 2023\n🏷️ **Category**: Over-Exploited\n💧 **Annual Extraction**: 165% of recharge\n⚠️ **Status**: Critical - Immediate action required\n\n**Key Issues:**\n• Intensive rice-wheat cultivation\n• Industrial water usage\n• Urban expansion in cities like Amritsar and Ludhiana\n\n**Recommended Actions:**\n• Implement drip irrigation systems\n• Crop diversification from water-intensive crops\n• Rainwater harvesting mandates`;
    hasData = true;
    suggestions = [
      'Show historical trends for Punjab',
      'Compare with neighboring Haryana',
      'What conservation methods work here?'
    ];
  }

  // Critical areas query
  else if (lowerMessage.includes('critical') || lowerMessage.includes('crisis') || lowerMessage.includes('emergency')) {
    intent = 'critical_areas';
    confidence = 0.95;
    responseMessage = `🚨 **CRITICAL GROUNDWATER AREAS - IMMEDIATE ATTENTION NEEDED**\n\n🔴 **OVER-EXPLOITED (>100% extraction):**\n• Punjab: 78% blocks affected\n• Haryana: 45% blocks affected\n• Tamil Nadu: Chennai region\n• Karnataka: Bangalore urban\n\n🟠 **CRITICAL (90-100% extraction):**\n• Maharashtra: Pune, Aurangabad\n• Gujarat: Ahmedabad\n• Rajasthan: Alwar district\n\n**Immediate Actions Required:**\n• Ban new bore wells in over-exploited areas\n• Implement water conservation policies\n• Promote sustainable agriculture practices`;
    hasData = true;
    suggestions = [
      'Show policy recommendations',
      'Compare with safe areas',
      'What are the economic impacts?'
    ];
  }

  // Comparison queries
  else if (lowerMessage.includes('compare') || lowerMessage.includes('vs') || lowerMessage.includes('versus')) {
    intent = 'comparison';
    confidence = 0.85;
    responseMessage = `📊 **GROUNDWATER COMPARISON ANALYSIS**\n\n🏆 **PUNJAB vs HARYANA (2023 Data)**\n\n**Punjab:**\n• Extraction Rate: 165%\n• Category: Over-Exploited\n• Main Issue: Rice-Wheat cultivation\n• Water Table Decline: 0.5-1m/year\n\n**Haryana:**\n• Extraction Rate: 145%\n• Category: Critical\n• Main Issue: Industrial + Agriculture\n• Water Table Decline: 0.3-0.7m/year\n\n**Key Differences:**\n• Punjab has higher extraction due to paddy cultivation\n• Haryana shows better groundwater management in some districts\n• Both states need urgent conservation measures\n\n**Recommendation:** Focus on crop diversification and efficient irrigation`;
    hasData = true;
    suggestions = [
      'Show trends for both states',
      'What policies work in Haryana?',
      'Compare with Maharashtra'
    ];
  }

  // Help queries
  else if (lowerMessage.includes('help') || lowerMessage.includes('commands') || lowerMessage.includes('what can you do')) {
    intent = 'help';
    confidence = 0.9;
    responseMessage = `🤖 **INGRES ChatBot Help & Commands**\n\nI'm your AI assistant for India's groundwater resources. I can help with:\n\n📊 **Data Queries:**\n• "Show me Punjab groundwater data"\n• "What's the status of Maharashtra?"\n• "Groundwater in Karnataka districts"\n\n📈 **Analysis & Trends:**\n• "Compare Punjab vs Haryana"\n• "Historical trends for Gujarat"\n• "Show rainfall correlation"\n\n⚠️ **Critical Areas:**\n• "Which areas are over-exploited?"\n• "Show critical regions"\n• "Water crisis areas"\n\n🏛️ **Policy & Solutions:**\n• "Policy recommendations for Punjab"\n• "Conservation methods"\n• "What should government do?"\n\n🌍 **Multi-language Support:**\n• English, Hindi, Gujarati, Marathi, Tamil, Telugu, Bengali, Kannada\n\nTry asking: "Show Punjab data" or "Compare states"`;
    suggestions = [
      'Show me Punjab data',
      'Compare two states',
      'What are critical areas?'
    ];
  }

  // Default response for unknown queries
  else {
    intent = 'unknown';
    confidence = 0.3;
    responseMessage = `I'm not sure I understood your query correctly. I can help you with:\n\n• "Show me [state] groundwater data"\n• "Compare [state1] and [state2]"\n• "Which areas are critical?"\n• "Policy recommendations"\n• "Help" for more options\n\nTry rephrasing your question or type "help" for detailed guidance.`;
    requiresClarification = true;
    suggestions = [
      'Show me Punjab data',
      'Which areas are critical?',
      'Help me understand'
    ];
  }

  const processingTime = Date.now() - startTime;

  return {
    message: responseMessage,
    intent: intent,
    confidence: confidence,
    processingTime: processingTime,
    hasData: hasData,
    requiresClarification: requiresClarification,
    suggestions: suggestions,
    dataSources: dataSources
  };
}