import React, { useState, useEffect, useRef } from 'react';
import './App.css';
import ParticleBackground from './ParticleBackground';
import FloatingActionButton from './FloatingActionButton';
import Shuffle from './Shuffle';
import HoverBorderGradient from './HoverBorderGradient';
import AceternityLogo from './AceternityLogo';

interface Message {
  id: number;
  type: 'user' | 'bot';
  content: string;
  timestamp: Date;
  suggestions?: string[];
  processingTime?: number;
  confidence?: number;
  groundwaterStatus?: string;
}

interface Conversation {
  id: string;
  title: string;
  messages: Message[];
  timestamp: Date;
}

const App: React.FC = () => {
  const [messages, setMessages] = useState<Message[]>([
    {
      id: 1,
      type: 'bot',
      content: `Welcome to INGRES ChatBot

I'm your AI assistant for India's groundwater resources. I can help you with:

• State-wise groundwater data and statistics
• Compare different states and regions
• Identify critical and over-exploited areas
• Historical trends and analysis
• Policy recommendations

Try asking: "Show Punjab data" or "Compare Maharashtra and Gujarat"`,
      timestamp: new Date(),
      suggestions: [
        'Show Punjab data',
        'Compare states',
        'Critical areas',
        'Help'
      ]
    }
  ]);

  const [inputValue, setInputValue] = useState('');
  const [isTyping, setIsTyping] = useState(false);
  const [isConnected] = useState(true);
  const [theme, setTheme] = useState<'light' | 'dark'>('light');
  const [conversations, setConversations] = useState<Conversation[]>([]);
  const [currentConversationId, setCurrentConversationId] = useState<string>('');
  const [showHistory, setShowHistory] = useState(false);
  const [showStats, setShowStats] = useState(false);
  const messagesEndRef = useRef<HTMLDivElement>(null);

  const scrollToBottom = () => {
    messagesEndRef.current?.scrollIntoView({ behavior: 'smooth' });
  };

  useEffect(() => {
    scrollToBottom();
  }, [messages]);

  useEffect(() => {
    // Load theme from localStorage
    try {
      const savedTheme = localStorage.getItem('ingres-theme') as 'light' | 'dark';
      if (savedTheme && (savedTheme === 'light' || savedTheme === 'dark')) {
        setTheme(savedTheme);
      }
    } catch (error) {
      console.warn('Failed to load theme from localStorage:', error);
    }

    // Load conversations from localStorage
    try {
      const savedConversations = localStorage.getItem('ingres-conversations');
      if (savedConversations) {
        const parsedConversations = JSON.parse(savedConversations);
        // Parse timestamps back to Date objects
        const conversationsWithDates = parsedConversations.map((conv: any) => ({
          ...conv,
          timestamp: new Date(conv.timestamp),
          messages: conv.messages.map((msg: any) => ({
            ...msg,
            timestamp: new Date(msg.timestamp)
          }))
        }));
        setConversations(conversationsWithDates);
      }
    } catch (error) {
      console.warn('Failed to load conversations from localStorage:', error);
    }

    // Create new conversation
    const newConversationId = Date.now().toString();
    setCurrentConversationId(newConversationId);
    const initialConversation: Conversation = {
      id: newConversationId,
      title: 'New Conversation',
      messages: messages,
      timestamp: new Date()
    };
    setConversations(prev => [initialConversation, ...(prev || [])]);
  }, []); // eslint-disable-line react-hooks/exhaustive-deps

  useEffect(() => {
    // Save conversations to localStorage whenever conversations change
    const saveConversations = () => {
      try {
        localStorage.setItem('ingres-conversations', JSON.stringify(conversations));
      } catch (error) {
        console.warn('Failed to save conversations to localStorage:', error);
      }
    };

    // Debounce the save operation
    const timeoutId = setTimeout(saveConversations, 100);
    return () => clearTimeout(timeoutId);
  }, [conversations]); // eslint-disable-line react-hooks/exhaustive-deps

  useEffect(() => {
    // Apply theme
    document.documentElement.setAttribute('data-theme', theme);
    try {
      localStorage.setItem('ingres-theme', theme);
    } catch (error) {
      console.warn('Failed to save theme to localStorage:', error);
    }
  }, [theme]);

  // Removed unused checkConnection function

  const sendMessage = async (message: string = inputValue) => {
    if (!message || !message.trim()) return;

    if (!isConnected) {
      const errorMessage: Message = {
        id: Date.now(),
        type: 'bot',
        content: '❌ Not connected to server. Please check your connection.',
        timestamp: new Date(),
      };
      setMessages(prev => [...prev, errorMessage]);
      return;
    }

    const userMessage: Message = {
      id: Date.now(),
      type: 'user',
      content: message.trim(),
      timestamp: new Date()
    };

    setMessages(prev => [...prev, userMessage]);
    setInputValue('');
    setIsTyping(true);

    try {
      const response = await fetch('/api/chat', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          message: message.trim(),
          timestamp: new Date().toISOString()
        })
      });

      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }

      const data = await response.json();

      const botMessage: Message = {
        id: Date.now() + 1,
        type: 'bot',
        content: data.message || 'Sorry, I encountered an error processing your request.',
        timestamp: new Date(),
        processingTime: data.processing_time_ms,
        confidence: data.confidence,
        suggestions: data.suggestions || [],
        groundwaterStatus: data.groundwater_status
      };

      setMessages(prev => [...prev, botMessage]);

      // Update conversation with new messages
      setConversations(prev => (prev || []).map(conv => {
        if (conv.id === currentConversationId) {
          const currentMessages = conv.messages || [];
          const updatedMessages = [...currentMessages, userMessage, botMessage];
          // Update title if it's the first user message
          const title = currentMessages.length === 1 && conv.title === 'New Conversation'
            ? (message.length > 30 ? message.substring(0, 30) + '...' : message)
            : conv.title;

          return {
            ...conv,
            title,
            messages: updatedMessages
          };
        }
        return conv;
      }));

    } catch (error) {
      const errorMessage: Message = {
        id: Date.now() + 1,
        type: 'bot',
        content: `❌ Sorry, I'm having trouble connecting to the server. Error: ${error instanceof Error ? error.message : 'Unknown error'}. Please check your connection and try again.`,
        timestamp: new Date(),
      };
      setMessages(prev => [...prev, errorMessage]);
    } finally {
      setIsTyping(false);
    }
  };

  const handleKeyPress = (e: React.KeyboardEvent) => {
    if (e.key === 'Enter' && !e.shiftKey) {
      e.preventDefault();
      sendMessage();
    }
  };

  const sendQuickQuery = (query: string) => {
    setInputValue(query);
    sendMessage(query);
  };

  const toggleTheme = () => {
    setTheme(prev => prev === 'light' ? 'dark' : 'light');
  };

  const formatTime = (timestamp: Date) => {
    return timestamp.toLocaleTimeString('en-US', {
      hour: '2-digit',
      minute: '2-digit'
    });
  };

  const formatMessage = (content: string) => {
    return content
      .replace(/\*\*(.*?)\*\*/g, '<strong>$1</strong>')
      .replace(/\*(.*?)\*/g, '<em>$1</em>')
      .replace(/\n/g, '<br>');
  };

  const startNewConversation = () => {
    const newConversationId = Date.now().toString();
    const newConversation: Conversation = {
      id: newConversationId,
      title: 'New Conversation',
      messages: [{
        id: 1,
        type: 'bot',
        content: `Welcome back to INGRES ChatBot

I'm ready to help you with India's groundwater data. What would you like to know?`,
        timestamp: new Date(),
        suggestions: [
          'Show Punjab data',
          'Compare states',
          'Critical areas',
          'Help'
        ]
      }],
      timestamp: new Date()
    };

    setConversations(prev => [newConversation, ...(prev || [])]);
    setCurrentConversationId(newConversationId);
    setMessages(newConversation.messages);
  };

  const loadConversation = (conversation: Conversation) => {
    setCurrentConversationId(conversation.id);
    setMessages(conversation.messages);
    setShowHistory(false);
  };

  const exportConversation = () => {
    try {
      if (!conversations || conversations.length === 0) {
        console.warn('No conversations available to export');
        return;
      }

      const conversation = conversations.find(c => c.id === currentConversationId);
      if (!conversation) {
        console.warn('Current conversation not found to export');
        return;
      }

      const exportData = {
        title: conversation.title,
        timestamp: conversation.timestamp,
        messages: conversation.messages.map(msg => ({
          type: msg.type,
          content: msg.content,
          timestamp: msg.timestamp,
          processingTime: msg.processingTime,
          confidence: msg.confidence
        }))
      };

      const blob = new Blob([JSON.stringify(exportData, null, 2)], { type: 'application/json' });
      const url = URL.createObjectURL(blob);
      const a = document.createElement('a');
      a.href = url;
      a.download = `ingres-conversation-${conversation.title.replace(/[^a-z0-9]/gi, '_').toLowerCase()}.json`;
      document.body.appendChild(a);
      a.click();
      document.body.removeChild(a);
      URL.revokeObjectURL(url);
    } catch (error) {
      console.error('Failed to export conversation:', error);
    }
  };

  return (
    <div className="app">
      {/* Particle Background */}
      <ParticleBackground />

      {/* Header */}
      <header className="header">
        <div className="header-content">
          <div className="header-left">
            <h1>
              <Shuffle
                text="🌊 INGRES ChatBot"
                shuffleDirection="right"
                duration={0.35}
                animationMode="evenodd"
                shuffleTimes={1}
                ease="power3.out"
                stagger={0.03}
                threshold={0.1}
                triggerOnce={true}
                triggerOnHover={true}
                respectReducedMotion={true}
              />
            </h1>
            <p>AI-Powered Groundwater Resource Assistant</p>
          </div>
          <div className="header-right">
            <div className="status-indicator">
              <span className={`status-dot ${isConnected ? 'online' : 'offline'}`}></span>
              <span className="status-text">
                {isConnected ? 'Connected' : 'Disconnected'}
              </span>
            </div>
            <button
              onClick={toggleTheme}
              className="theme-toggle"
              title={`Switch to ${theme === 'light' ? 'dark' : 'light'} mode`}
            >
              {theme === 'light' ? '🌙' : '☀️'}
            </button>
            <button
              onClick={() => setShowHistory(!showHistory)}
              className="history-toggle"
              title="Conversation History"
            >
              📚
            </button>
            <button
              onClick={() => setShowStats(!showStats)}
              className="stats-toggle"
              title="Statistics Dashboard"
            >
              📊
            </button>
          </div>
        </div>
      </header>

      <div className="main-content">
        {/* Conversation History Sidebar */}
        {showHistory && (
          <div className="history-sidebar">
            <div className="history-header">
              <h3>Conversation History</h3>
              <button onClick={startNewConversation} className="new-conversation-btn">
                ➕ New Chat
              </button>
            </div>
            <div className="history-list">
              {conversations && conversations.length > 0 ? (
                conversations.map(conversation => (
                  <div
                    key={conversation.id}
                    className={`history-item ${conversation.id === currentConversationId ? 'active' : ''}`}
                    onClick={() => loadConversation(conversation)}
                  >
                    <div className="history-title">{conversation.title}</div>
                    <div className="history-time">
                      {conversation.timestamp.toLocaleDateString()}
                    </div>
                    <div className="history-preview">
                      {conversation.messages ? conversation.messages.length : 0} messages
                    </div>
                  </div>
                ))
              ) : (
                <div className="history-item">
                  <div className="history-title">No conversations yet</div>
                  <div className="history-preview">Start chatting to create conversations</div>
                </div>
              )}
            </div>
          </div>
        )}

        {/* Statistics Dashboard */}
        {showStats && (
          <div className="stats-sidebar">
            <div className="stats-header">
              <h3>Groundwater Statistics</h3>
            </div>
            <div className="stats-content">
              <div className="stat-card">
                <div className="stat-icon">🌍</div>
                <div className="stat-info">
                  <div className="stat-value">36</div>
                  <div className="stat-label">States & Territories</div>
                </div>
              </div>
              <div className="stat-card">
                <div className="stat-icon">⚠️</div>
                <div className="stat-info">
                  <div className="stat-value">78%</div>
                  <div className="stat-label">Punjab Over-exploited</div>
                </div>
              </div>
              <div className="stat-card">
                <div className="stat-icon">💧</div>
                <div className="stat-info">
                  <div className="stat-value">₹2.5L Cr</div>
                  <div className="stat-label">Annual Economic Impact</div>
                </div>
              </div>
              <div className="stat-card">
                <div className="stat-icon">📈</div>
                <div className="stat-info">
                  <div className="stat-value">600M</div>
                  <div className="stat-label">People Affected</div>
                </div>
              </div>
            </div>
          </div>
        )}

        {/* Chat Container */}
        <main className="chat-container" role="main" aria-label="Chat conversation">
          <div className="chat-messages" role="log" aria-label="Chat messages" aria-live="polite">
            {messages.map((message) => (
              <div key={message.id} className={`message ${message.type}`}>
                <div className="message-avatar">
                  {message.type === 'user' ? '👤' : '🤖'}
                </div>
                <div
                  className={`message-content ${message.groundwaterStatus ? `status-${message.groundwaterStatus}` : ''}`}
                >
                  <div
                    className="message-text"
                    dangerouslySetInnerHTML={{ __html: formatMessage(message.content) }}
                  />
                  <div className="message-meta">
                    <span className="message-time">{formatTime(message.timestamp)}</span>
                    {message.processingTime && (
                      <span className="message-processing-time">
                        • {message.processingTime}ms
                      </span>
                    )}
                    {message.confidence && (
                      <span className="message-confidence">
                        • {(message.confidence * 100).toFixed(0)}% confidence
                      </span>
                    )}
                  </div>
                  {message.suggestions && message.suggestions.length > 0 && (
                    <div className="message-suggestions">
                      {message.suggestions.map((suggestion, index) => (
                        <button
                          key={index}
                          className="suggestion-btn"
                          onClick={() => sendQuickQuery(suggestion)}
                        >
                          {suggestion}
                        </button>
                      ))}
                    </div>
                  )}
                </div>
              </div>
            ))}

            {isTyping && (
              <div className="message bot typing">
                <div className="message-avatar">🤖</div>
                <div className="message-content">
                  <div className="typing-indicator">
                    <span></span>
                    <span></span>
                    <span></span>
                  </div>
                </div>
              </div>
            )}

            <div ref={messagesEndRef} />
          </div>

          {/* Quick Actions */}
          <div className="quick-actions" role="group" aria-label="Quick actions">
            <button
              className="quick-btn"
              onClick={() => sendQuickQuery('Show groundwater data for Punjab')}
              aria-label="Show Punjab groundwater data"
            >
              📊 Punjab Data
            </button>
            <button
              className="quick-btn"
              onClick={() => sendQuickQuery('Compare Maharashtra and Gujarat')}
              aria-label="Compare Maharashtra and Gujarat"
            >
              🔄 Compare States
            </button>
            <button
              className="quick-btn"
              onClick={() => sendQuickQuery('Show critical areas')}
              aria-label="Show critical groundwater areas"
            >
              ⚠️ Critical Areas
            </button>
            <button
              className="quick-btn"
              onClick={() => sendQuickQuery('Help me understand groundwater categories')}
              aria-label="Get help understanding groundwater categories"
            >
              ❓ Help
            </button>
          </div>

          {/* Input Container */}
          <div className="input-container">
            <div className="input-wrapper">
              <input
                type="text"
                value={inputValue}
                onChange={(e) => setInputValue(e.target.value)}
                onKeyPress={handleKeyPress}
                placeholder="Ask about groundwater data..."
                maxLength={500}
                disabled={!isConnected}
                aria-label="Chat input"
                autoComplete="off"
              />
              <button
                className="send-btn"
                onClick={() => sendMessage()}
                disabled={!inputValue.trim() || isTyping}
                title={isConnected ? "Send message" : "Waiting for connection"}
                aria-label="Send message"
              >
                {isTyping ? '⏳' : '📤'}
              </button>
            </div>
            <div className="input-info">
              <span>{inputValue.length}/500</span>
              {!isConnected && <span className="connection-status">Disconnected</span>}
              {isTyping && <span className="typing-status">Bot is typing...</span>}
            </div>
          </div>
        </main>
      </div>

      {/* Export Button and Floating Actions */}
      <div className="export-container">
        <HoverBorderGradient
          onClick={exportConversation}
          containerClassName="export-gradient-btn"
          className="export-gradient-content"
        >
          <AceternityLogo />
          <span>💾 Export Conversation</span>
        </HoverBorderGradient>
      </div>

      {/* Floating Action Buttons */}
      <div className="fab-group">
        <FloatingActionButton
          onClick={() => sendQuickQuery('Show groundwater data for Punjab')}
          icon="📊"
          label="Punjab Data"
          color="#2563eb"
        />
        <FloatingActionButton
          onClick={() => sendQuickQuery('Compare Maharashtra and Gujarat')}
          icon="🔄"
          label="Compare States"
          color="#059669"
        />
        <FloatingActionButton
          onClick={() => sendQuickQuery('Show critical areas')}
          icon="⚠️"
          label="Critical Areas"
          color="#dc2626"
        />
        <FloatingActionButton
          onClick={() => sendQuickQuery('Help me understand groundwater categories')}
          icon="❓"
          label="Help"
          color="#8b5cf6"
        />
      </div>

      {/* Footer */}
      <footer className="footer">
        <p>Smart India Hackathon 2025 | Enhanced Edition with AI-Powered Features</p>
      </footer>
    </div>
  );
};

export default App;
