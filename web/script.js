// INGRES ChatBot - Frontend JavaScript

class INGRESChatBot {
    constructor() {
        this.isConnected = false;
        this.isTyping = false;
        this.messageHistory = [];
        this.currentChart = null;
        this.isListening = false;
        this.recognition = null;
        this.synth = null;
        this.currentLanguage = 'en';

        this.initializeElements();
        this.setupEventListeners();
        this.initializeVoice();
        this.checkConnection();
        this.showWelcomeMessage();
    }

    initializeElements() {
        this.chatMessages = document.getElementById('chatMessages');
        this.userInput = document.getElementById('userInput');
        this.sendButton = document.getElementById('sendButton');
        this.voiceButton = document.getElementById('voiceButton');
        this.voiceIcon = document.getElementById('voiceIcon');
        this.connectionStatus = document.getElementById('connectionStatus');
        this.statusText = document.getElementById('statusText');
        this.charCount = document.getElementById('charCount');
        this.typingIndicator = document.getElementById('typingIndicator');
        this.loadingOverlay = document.getElementById('loadingOverlay');
    }

    setupEventListeners() {
        // Send message on Enter key
        this.userInput.addEventListener('keypress', (e) => {
            if (e.key === 'Enter' && !e.shiftKey) {
                e.preventDefault();
                this.sendMessage();
            }
        });

        // Character count update
        this.userInput.addEventListener('input', () => {
            const length = this.userInput.value.length;
            this.charCount.textContent = `${length}/500`;
            
            // Enable/disable send button
            this.sendButton.disabled = length === 0 || !this.isConnected;
        });

        // Auto-resize input (if needed for multiline)
        this.userInput.addEventListener('input', () => {
            this.userInput.style.height = 'auto';
            this.userInput.style.height = this.userInput.scrollHeight + 'px';
        });
    }

    async checkConnection() {
        try {
            const response = await fetch('/api/status');
            if (response.ok) {
                this.setConnectionStatus(true);
            } else {
                this.setConnectionStatus(false);
            }
        } catch (error) {
            console.error('Connection check failed:', error);
            this.setConnectionStatus(false);
        }
    }

    setConnectionStatus(connected) {
        this.isConnected = connected;
        
        if (connected) {
            this.connectionStatus.className = 'status-dot online';
            this.statusText.textContent = 'Connected';
            this.sendButton.disabled = this.userInput.value.length === 0;
        } else {
            this.connectionStatus.className = 'status-dot offline';
            this.statusText.textContent = 'Disconnected';
            this.sendButton.disabled = true;
        }
    }

    showWelcomeMessage() {
        const welcomeMessage = {
            type: 'bot',
            content: `🌊 Welcome to INGRES ChatBot!

I'm your AI assistant for groundwater resource information across India. I can help you with:

📊 **Groundwater Data Queries**
• "Show me groundwater data for Punjab"
• "What's the status of Maharashtra's water resources?"

📈 **Historical Analysis**  
• "Compare groundwater trends for Gujarat vs Rajasthan"
• "Show me 5-year data for Karnataka"

⚠️ **Critical Area Identification**
• "Which areas are over-exploited?"
• "Show me safe groundwater zones"

❓ **General Help**
• "Explain groundwater categories"
• "What does stage of extraction mean?"

Try asking me anything about India's groundwater resources!`,
            timestamp: new Date()
        };
        
        this.addMessage(welcomeMessage);
    }

    async sendMessage() {
        const message = this.userInput.value.trim();
        if (!message || !this.isConnected) return;

        // Check authentication
        if (!this.isAuthenticated()) {
            this.showAuthModal();
            return;
        }

        // Add user message to chat
        this.addMessage({
            type: 'user',
            content: message,
            timestamp: new Date()
        });

        // Clear input
        this.userInput.value = '';
        this.charCount.textContent = '0/500';
        this.sendButton.disabled = true;

        // Show typing indicator
        this.showTypingIndicator();

        try {
            // Send request to backend with authentication
            const response = await fetch('/api/chat', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                    'Authorization': `Bearer ${this.getAuthToken()}`
                },
                body: JSON.stringify({
                    message: message,
                    timestamp: new Date().toISOString()
                })
            });

            if (response.status === 401) {
                // Token expired or invalid
                this.logout();
                this.showAuthModal();
                throw new Error('Authentication required');
            }

            if (response.status === 429) {
                // Rate limit exceeded
                const data = await response.json();
                throw new Error(data.message || 'Rate limit exceeded');
            }

            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }

            const data = await response.json();

            // Hide typing indicator
            this.hideTypingIndicator();

            // Add bot response
            this.addMessage({
                type: 'bot',
                content: data.message || 'Sorry, I encountered an error processing your request.',
                timestamp: new Date(),
                processingTime: data.processing_time_ms
            });

        } catch (error) {
            console.error('Error sending message:', error);
            this.hideTypingIndicator();

            let errorMessage = '❌ Sorry, I\'m having trouble connecting to the server. Please check your connection and try again.';

            if (error.message.includes('Authentication required')) {
                errorMessage = '❌ Please login to continue using the chat service.';
            } else if (error.message.includes('Rate limit')) {
                errorMessage = `❌ ${error.message}`;
            }

            this.addMessage({
                type: 'bot',
                content: errorMessage,
                timestamp: new Date(),
                isError: true
            });
        }

        // Re-enable input
        this.sendButton.disabled = false;
    }

    sendQuickQuery(query) {
        this.userInput.value = query;
        this.sendMessage();
    }

    addMessage(message) {
        const messageElement = document.createElement('div');
        messageElement.className = `message ${message.type}`;
        
        const avatar = document.createElement('div');
        avatar.className = 'message-avatar';
        avatar.textContent = message.type === 'user' ? '👤' : '🤖';
        
        const content = document.createElement('div');
        content.className = 'message-content';
        
        // Format message content
        if (message.content.includes('\n')) {
            // Handle multiline messages
            content.innerHTML = this.formatMessage(message.content);
        } else {
            content.textContent = message.content;
        }
        
        const time = document.createElement('div');
        time.className = 'message-time';
        time.textContent = this.formatTime(message.timestamp);

        if (message.processingTime) {
            time.textContent += ` • ${message.processingTime}ms`;
        }

        // Add voice button for bot messages
        if (message.type === 'bot' && this.synth) {
            const voiceBtn = document.createElement('button');
            voiceBtn.className = 'voice-response-btn';
            voiceBtn.innerHTML = '🔊';
            voiceBtn.title = 'Listen to response';
            voiceBtn.onclick = () => this.speakText(message.content);
            voiceBtn.style.cssText = `
                background: none;
                border: none;
                cursor: pointer;
                font-size: 1rem;
                margin-left: 8px;
                opacity: 0.7;
                transition: opacity 0.2s ease;
            `;
            voiceBtn.onmouseover = () => voiceBtn.style.opacity = '1';
            voiceBtn.onmouseout = () => voiceBtn.style.opacity = '0.7';
            time.appendChild(voiceBtn);
        }

        messageElement.appendChild(avatar);
        messageElement.appendChild(content);
        content.appendChild(time);
        
        this.chatMessages.appendChild(messageElement);
        this.scrollToBottom();
        
        // Store in history
        this.messageHistory.push(message);
    }

    formatMessage(content) {
        // Simple formatting for bot messages
        return content
            .replace(/\*\*(.*?)\*\*/g, '<strong>$1</strong>')  // Bold
            .replace(/\*(.*?)\*/g, '<em>$1</em>')              // Italic
            .replace(/\n/g, '<br>')                            // Line breaks
            .replace(/•/g, '&bull;');                          // Bullet points
    }

    formatTime(timestamp) {
        return timestamp.toLocaleTimeString('en-US', {
            hour: '2-digit',
            minute: '2-digit'
        });
    }

    showTypingIndicator() {
        this.isTyping = true;
        this.typingIndicator.classList.remove('hidden');
    }

    hideTypingIndicator() {
        this.isTyping = false;
        this.typingIndicator.classList.add('hidden');
    }

    scrollToBottom() {
        this.chatMessages.scrollTop = this.chatMessages.scrollHeight;
    }

    showLoading() {
        this.loadingOverlay.classList.remove('hidden');
    }

    hideLoading() {
        this.loadingOverlay.classList.add('hidden');
    }

    // Data Visualization Functions
    showVisualization() {
        const panel = document.getElementById('visualizationPanel');
        panel.classList.remove('hidden');
        this.createDefaultChart();
        this.scrollToBottom();
    }

    closeVisualization() {
        const panel = document.getElementById('visualizationPanel');
        panel.classList.add('hidden');
        if (this.currentChart) {
            this.currentChart.destroy();
            this.currentChart = null;
        }
    }

    createDefaultChart() {
        const ctx = document.getElementById('dataChart').getContext('2d');

        // Sample data for demonstration
        const data = {
            labels: ['Punjab', 'Haryana', 'Maharashtra', 'Gujarat', 'Rajasthan', 'Karnataka'],
            datasets: [{
                label: 'Stage of Extraction (%)',
                data: [113.8, 167.6, 169.2, 120.5, 145.8, 155.3],
                backgroundColor: [
                    'rgba(255, 99, 132, 0.6)',
                    'rgba(54, 162, 235, 0.6)',
                    'rgba(255, 205, 86, 0.6)',
                    'rgba(75, 192, 192, 0.6)',
                    'rgba(153, 102, 255, 0.6)',
                    'rgba(255, 159, 64, 0.6)'
                ],
                borderColor: [
                    'rgba(255, 99, 132, 1)',
                    'rgba(54, 162, 235, 1)',
                    'rgba(255, 205, 86, 1)',
                    'rgba(75, 192, 192, 1)',
                    'rgba(153, 102, 255, 1)',
                    'rgba(255, 159, 64, 1)'
                ],
                borderWidth: 2
            }]
        };

        const config = {
            type: 'bar',
            data: data,
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    title: {
                        display: true,
                        text: 'Groundwater Stage of Extraction by State',
                        font: {
                            size: 16,
                            weight: 'bold'
                        }
                    },
                    legend: {
                        display: false
                    }
                },
                scales: {
                    y: {
                        beginAtZero: true,
                        title: {
                            display: true,
                            text: 'Stage of Extraction (%)'
                        }
                    },
                    x: {
                        title: {
                            display: true,
                            text: 'States'
                        }
                    }
                }
            }
        };

        if (this.currentChart) {
            this.currentChart.destroy();
        }
        this.currentChart = new Chart(ctx, config);
    }

    // Voice Interface Functions
    initializeVoice() {
        // Check for browser support
        if ('webkitSpeechRecognition' in window || 'SpeechRecognition' in window) {
            const SpeechRecognition = window.SpeechRecognition || window.webkitSpeechRecognition;
            this.recognition = new SpeechRecognition();
            this.recognition.continuous = false;
            this.recognition.interimResults = false;
            this.recognition.lang = 'en-IN'; // Indian English

            this.recognition.onstart = () => {
                this.isListening = true;
                this.voiceButton.classList.add('listening');
                this.voiceIcon.textContent = '🎙️';
                this.showVoiceStatus('Listening...');
            };

            this.recognition.onresult = (event) => {
                const transcript = event.results[0][0].transcript;
                this.userInput.value = transcript;
                this.sendMessage();
            };

            this.recognition.onend = () => {
                this.stopListening();
            };

            this.recognition.onerror = (event) => {
                console.error('Speech recognition error:', event.error);
                this.stopListening();
                this.showVoiceStatus('Voice recognition failed. Try again.');
            };
        } else {
            // Disable voice button if not supported
            this.voiceButton.disabled = true;
            this.voiceButton.title = 'Voice input not supported in this browser';
        }

        // Initialize speech synthesis
        if ('speechSynthesis' in window) {
            this.synth = window.speechSynthesis;
        }
    }

    toggleVoiceInput() {
        if (!this.recognition) {
            alert('Voice input is not supported in your browser. Please use Chrome, Edge, or Safari.');
            return;
        }

        if (this.isListening) {
            this.stopListening();
        } else {
            this.startListening();
        }
    }

    startListening() {
        try {
            this.recognition.start();
        } catch (error) {
            console.error('Error starting voice recognition:', error);
            this.showVoiceStatus('Could not start voice recognition');
        }
    }

    stopListening() {
        if (this.recognition && this.isListening) {
            this.recognition.stop();
        }
        this.isListening = false;
        this.voiceButton.classList.remove('listening');
        this.voiceIcon.textContent = '🎤';
        this.hideVoiceStatus();
    }

    showVoiceStatus(message) {
        // Remove existing status if any
        const existingStatus = document.querySelector('.voice-status');
        if (existingStatus) {
            existingStatus.remove();
        }

        const statusDiv = document.createElement('div');
        statusDiv.className = 'voice-status';
        statusDiv.textContent = message;
        statusDiv.style.cssText = `
            position: fixed;
            top: 20px;
            left: 50%;
            transform: translateX(-50%);
            background: var(--primary-color, #2563eb);
            color: white;
            padding: 10px 20px;
            border-radius: 20px;
            font-size: 14px;
            z-index: 1000;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
        `;

        document.body.appendChild(statusDiv);

        // Auto-hide after 3 seconds
        setTimeout(() => {
            if (statusDiv.parentNode) {
                statusDiv.remove();
            }
        }, 3000);
    }

    hideVoiceStatus() {
        const statusDiv = document.querySelector('.voice-status');
        if (statusDiv) {
            statusDiv.remove();
        }
    }

    speakText(text) {
        if (!this.synth) {
            console.warn('Speech synthesis not supported');
            return;
        }

        // Cancel any ongoing speech
        this.synth.cancel();

        const utterance = new SpeechSynthesisUtterance(text);
        utterance.lang = 'en-IN'; // Indian English
        utterance.rate = 0.9; // Slightly slower for clarity
        utterance.pitch = 1;

        // Add voice status
        utterance.onstart = () => {
            this.showVoiceStatus('Speaking...');
        };

        utterance.onend = () => {
            this.hideVoiceStatus();
        };

        utterance.onerror = (event) => {
            console.error('Speech synthesis error:', event);
            this.hideVoiceStatus();
        };

        this.synth.speak(utterance);
    }

    // Language switching functionality
    changeLanguage() {
        const languageSelect = document.getElementById('languageSelect');
        const newLanguage = languageSelect.value;

        if (newLanguage !== this.currentLanguage) {
            this.currentLanguage = newLanguage;
            this.updateUILanguage();
            this.updateVoiceLanguage();

            // Show language change notification
            this.showVoiceStatus(`Language changed to ${this.getLanguageName(newLanguage)}`);
            setTimeout(() => this.hideVoiceStatus(), 2000);
        }
    }

    // Enhanced visualization functions
    showVisualization() {
        if (!this.isAuthenticated()) {
            this.showAuthModal();
            return;
        }

        const panel = document.getElementById('visualizationPanel');
        panel.classList.remove('hidden');
        this.createCharts();
        this.updateAnalytics();
        this.connectWebSocket();
    }

    closeVisualization() {
        const panel = document.getElementById('visualizationPanel');
        panel.classList.add('hidden');
        this.disconnectWebSocket();
    }

    // Authentication functions
    isAuthenticated() {
        return !!localStorage.getItem('ingres_token');
    }

    getAuthToken() {
        return localStorage.getItem('ingres_token');
    }

    getCurrentUser() {
        const token = this.getAuthToken();
        if (!token) return null;

        try {
            const payload = JSON.parse(atob(token.split('.')[1]));
            return payload;
        } catch (error) {
            return null;
        }
    }

    showAuthModal() {
        const modal = document.getElementById('authModal');
        modal.classList.remove('hidden');
        document.getElementById('loginForm').classList.remove('hidden');
        document.getElementById('registerForm').classList.add('hidden');
        document.getElementById('modalTitle').textContent = 'Login to INGRES';
    }

    closeAuthModal() {
        const modal = document.getElementById('authModal');
        modal.classList.add('hidden');
    }

    showRegisterForm() {
        document.getElementById('loginForm').classList.add('hidden');
        document.getElementById('registerForm').classList.remove('hidden');
        document.getElementById('modalTitle').textContent = 'Register for INGRES';
    }

    showLoginForm() {
        document.getElementById('registerForm').classList.add('hidden');
        document.getElementById('loginForm').classList.remove('hidden');
        document.getElementById('modalTitle').textContent = 'Login to INGRES';
    }

    async login() {
        const username = document.getElementById('username').value;
        const password = document.getElementById('password').value;

        if (!username || !password) {
            this.showVoiceStatus('Please fill in all fields');
            setTimeout(() => this.hideVoiceStatus(), 3000);
            return;
        }

        try {
            const response = await fetch('/api/auth?action=login', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ username, password })
            });

            const data = await response.json();

            if (response.ok && data.success) {
                localStorage.setItem('ingres_token', data.token);
                localStorage.setItem('ingres_user', JSON.stringify(data.user));
                this.updateAuthUI();
                this.closeAuthModal();
                this.showVoiceStatus(`Welcome back, ${data.user.name}!`);
                setTimeout(() => this.hideVoiceStatus(), 3000);
            } else {
                this.showVoiceStatus(data.error || 'Login failed');
                setTimeout(() => this.hideVoiceStatus(), 3000);
            }
        } catch (error) {
            console.error('Login error:', error);
            this.showVoiceStatus('Network error. Please try again.');
            setTimeout(() => this.hideVoiceStatus(), 3000);
        }
    }

    async register() {
        const name = document.getElementById('regName').value;
        const username = document.getElementById('regUsername').value;
        const password = document.getElementById('regPassword').value;

        if (!name || !username || !password) {
            this.showVoiceStatus('Please fill in all fields');
            setTimeout(() => this.hideVoiceStatus(), 3000);
            return;
        }

        try {
            const response = await fetch('/api/auth?action=register', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ name, username, password })
            });

            const data = await response.json();

            if (response.ok && data.success) {
                localStorage.setItem('ingres_token', data.token);
                localStorage.setItem('ingres_user', JSON.stringify(data.user));
                this.updateAuthUI();
                this.closeAuthModal();
                this.showVoiceStatus(`Welcome to INGRES, ${data.user.name}!`);
                setTimeout(() => this.hideVoiceStatus(), 3000);
            } else {
                this.showVoiceStatus(data.error || 'Registration failed');
                setTimeout(() => this.hideVoiceStatus(), 3000);
            }
        } catch (error) {
            console.error('Registration error:', error);
            this.showVoiceStatus('Network error. Please try again.');
            setTimeout(() => this.hideVoiceStatus(), 3000);
        }
    }

    logout() {
        localStorage.removeItem('ingres_token');
        localStorage.removeItem('ingres_user');
        this.updateAuthUI();
        this.showVoiceStatus('Logged out successfully');
        setTimeout(() => this.hideVoiceStatus(), 3000);
    }

    updateAuthUI() {
        const userInfo = document.getElementById('userInfo');
        const loginBtn = document.getElementById('loginBtn');
        const userName = document.getElementById('userName');

        if (this.isAuthenticated()) {
            const user = this.getCurrentUser();
            if (user) {
                userName.textContent = user.name;
                userInfo.classList.remove('hidden');
                loginBtn.classList.add('hidden');
            }
        } else {
            userInfo.classList.add('hidden');
            loginBtn.classList.remove('hidden');
        }
    }

    async validateToken() {
        try {
            const response = await fetch('/api/auth?action=verify', {
                method: 'GET',
                headers: {
                    'Authorization': `Bearer ${this.getAuthToken()}`
                }
            });

            if (!response.ok) {
                // Token is invalid or expired
                this.logout();
                this.showVoiceStatus('Session expired. Please login again.');
                setTimeout(() => this.hideVoiceStatus(), 3000);
            }
        } catch (error) {
            console.error('Token validation error:', error);
        }
    }

    // WebSocket for real-time data synchronization
    connectWebSocket() {
        if (this.ws && this.ws.readyState === WebSocket.OPEN) {
            return; // Already connected
        }

        try {
            // For Vercel deployment, use the current host
            const wsUrl = window.location.protocol === 'https:'
                ? `wss://${window.location.host}/api/websocket`
                : `ws://${window.location.host}/api/websocket`;

            this.ws = new WebSocket(wsUrl);

            this.ws.onopen = (event) => {
                console.log('WebSocket connected');
                this.showVoiceStatus('Real-time data connected');

                // Subscribe to data streams
                this.ws.send(JSON.stringify({
                    type: 'subscribe',
                    streams: ['groundwater_data', 'analytics', 'alerts']
                }));

                setTimeout(() => this.hideVoiceStatus(), 2000);
            };

            this.ws.onmessage = (event) => {
                try {
                    const data = JSON.parse(event.data);
                    this.handleRealTimeUpdate(data);
                } catch (error) {
                    console.error('WebSocket message parse error:', error);
                }
            };

            this.ws.onclose = (event) => {
                console.log('WebSocket disconnected');
                this.showVoiceStatus('Real-time data disconnected');

                // Attempt to reconnect after 5 seconds
                setTimeout(() => {
                    if (!this.ws || this.ws.readyState !== WebSocket.OPEN) {
                        this.connectWebSocket();
                    }
                }, 5000);
            };

            this.ws.onerror = (error) => {
                console.error('WebSocket error:', error);
                this.showVoiceStatus('Real-time connection failed');
                setTimeout(() => this.hideVoiceStatus(), 3000);
            };

        } catch (error) {
            console.error('WebSocket connection failed:', error);
            this.showVoiceStatus('Real-time features unavailable');
            setTimeout(() => this.hideVoiceStatus(), 3000);
        }
    }

    disconnectWebSocket() {
        if (this.ws) {
            this.ws.close();
            this.ws = null;
        }
    }

    handleRealTimeUpdate(data) {
        switch (data.type) {
            case 'data_update':
                this.updateRealTimeData(data.data);
                break;
            case 'welcome':
                console.log('Real-time connection established:', data.message);
                break;
            case 'subscribed':
                console.log('Subscribed to streams:', data.streams);
                break;
            case 'alert':
                this.showAlert(data.alert);
                break;
            default:
                console.log('Unknown real-time update type:', data.type);
        }
    }

    updateRealTimeData(data) {
        // Update groundwater levels in real-time
        if (data.groundwater_levels) {
            this.updateGroundwaterLevels(data.groundwater_levels);
        }

        // Update rainfall data
        if (data.rainfall) {
            this.updateRainfallData(data.rainfall);
        }

        // Handle alerts
        if (data.alerts && data.alerts.length > 0) {
            data.alerts.forEach(alert => this.showAlert(alert));
        }

        // Update timestamp
        const timestamp = new Date(data.timestamp);
        this.updateLastUpdateTime(timestamp);
    }

    updateGroundwaterLevels(levels) {
        // Update chart data in real-time
        if (this.extractionChart) {
            const datasets = this.extractionChart.data.datasets[0];
            const labels = this.extractionChart.data.labels;

            // Update data points
            labels.forEach((label, index) => {
                const stateKey = label.toLowerCase();
                if (levels[stateKey]) {
                    // Simulate extraction rate based on water level
                    const level = levels[stateKey].level;
                    const extractionRate = Math.max(50, Math.min(200, (30 - level) * 10));
                    datasets.data[index] = extractionRate;
                }
            });

            this.extractionChart.update('none'); // Update without animation for real-time feel
        }

        // Update analytics summary
        if (levels.punjab) {
            document.getElementById('criticalState').textContent =
                `Punjab (${Math.round((30 - levels.punjab.level) * 10)}% extraction)`;
        }
        if (levels.kerala) {
            document.getElementById('safeState').textContent =
                `Kerala (${Math.round((30 - levels.kerala.level) * 8)}% extraction)`;
        }
    }

    updateRainfallData(rainfall) {
        // Update correlation chart with real rainfall data
        if (this.correlationChart) {
            // Add current rainfall point to correlation chart
            const newPoint = {
                x: rainfall.current,
                y: 125 // Average extraction rate
            };

            // Update the dataset
            this.correlationChart.data.datasets[0].data.push(newPoint);
            if (this.correlationChart.data.datasets[0].data.length > 10) {
                this.correlationChart.data.datasets[0].data.shift(); // Keep only last 10 points
            }

            this.correlationChart.update('none');
        }
    }

    showAlert(alert) {
        // Create alert notification
        const alertDiv = document.createElement('div');
        alertDiv.className = `alert alert-${alert.severity || 'info'}`;
        alertDiv.innerHTML = `
            <div class="alert-icon">🚨</div>
            <div class="alert-content">
                <div class="alert-title">${alert.location}: ${alert.message}</div>
                <div class="alert-time">${new Date().toLocaleTimeString()}</div>
            </div>
            <button onclick="this.parentElement.remove()">×</button>
        `;

        // Add to alerts container (create if doesn't exist)
        let alertsContainer = document.getElementById('alertsContainer');
        if (!alertsContainer) {
            alertsContainer = document.createElement('div');
            alertsContainer.id = 'alertsContainer';
            alertsContainer.className = 'alerts-container';
            document.body.appendChild(alertsContainer);
        }

        alertsContainer.appendChild(alertDiv);

        // Auto-remove after 10 seconds
        setTimeout(() => {
            if (alertDiv.parentElement) {
                alertDiv.remove();
            }
        }, 10000);
    }

    updateLastUpdateTime(timestamp) {
        const timeElement = document.getElementById('lastUpdateTime');
        if (timeElement) {
            timeElement.textContent = timestamp.toLocaleTimeString();
        } else {
            // Create last update indicator
            const indicator = document.createElement('div');
            indicator.id = 'lastUpdateTime';
            indicator.className = 'last-update-indicator';
            indicator.innerHTML = `🔄 Last updated: ${timestamp.toLocaleTimeString()}`;
            document.querySelector('.status-indicator').appendChild(indicator);
        }
    }

    createCharts() {
        this.createExtractionChart();
        this.createTrendChart();
        this.createCategoryChart();
        this.createCorrelationChart();
    }

    createExtractionChart() {
        const ctx = document.getElementById('extractionChart').getContext('2d');

        const data = {
            labels: ['Punjab', 'Haryana', 'Maharashtra', 'Gujarat', 'Tamil Nadu', 'Karnataka', 'Rajasthan', 'Kerala'],
            datasets: [{
                label: 'Groundwater Extraction Rate (%)',
                data: [165, 145, 156, 134, 167, 155, 142, 65],
                backgroundColor: [
                    'rgba(255, 99, 132, 0.8)',
                    'rgba(255, 159, 64, 0.8)',
                    'rgba(255, 205, 86, 0.8)',
                    'rgba(75, 192, 192, 0.8)',
                    'rgba(54, 162, 235, 0.8)',
                    'rgba(153, 102, 255, 0.8)',
                    'rgba(201, 203, 207, 0.8)',
                    'rgba(40, 167, 69, 0.8)'
                ],
                borderColor: [
                    'rgb(255, 99, 132)',
                    'rgb(255, 159, 64)',
                    'rgb(255, 205, 86)',
                    'rgb(75, 192, 192)',
                    'rgb(54, 162, 235)',
                    'rgb(153, 102, 255)',
                    'rgb(201, 203, 207)',
                    'rgb(40, 167, 69)'
                ],
                borderWidth: 2
            }]
        };

        const config = {
            type: 'bar',
            data: data,
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    legend: {
                        position: 'top',
                    },
                    title: {
                        display: true,
                        text: 'State-wise Groundwater Extraction Rates (2023)'
                    }
                },
                scales: {
                    y: {
                        beginAtZero: true,
                        max: 200,
                        title: {
                            display: true,
                            text: 'Extraction Rate (%)'
                        }
                    }
                }
            }
        };

        if (this.extractionChart) {
            this.extractionChart.destroy();
        }
        this.extractionChart = new Chart(ctx, config);
    }

    createTrendChart() {
        const ctx = document.getElementById('trendChart').getContext('2d');

        const data = {
            labels: ['2019', '2020', '2021', '2022', '2023'],
            datasets: [{
                label: 'Punjab',
                data: [140, 150, 155, 160, 165],
                borderColor: 'rgb(255, 99, 132)',
                backgroundColor: 'rgba(255, 99, 132, 0.1)',
                tension: 0.4,
                fill: true
            }, {
                label: 'Haryana',
                data: [125, 135, 140, 142, 145],
                borderColor: 'rgb(255, 159, 64)',
                backgroundColor: 'rgba(255, 159, 64, 0.1)',
                tension: 0.4,
                fill: true
            }, {
                label: 'Maharashtra',
                data: [130, 140, 148, 152, 156],
                borderColor: 'rgb(255, 205, 86)',
                backgroundColor: 'rgba(255, 205, 86, 0.1)',
                tension: 0.4,
                fill: true
            }]
        };

        const config = {
            type: 'line',
            data: data,
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    title: {
                        display: true,
                        text: 'Groundwater Extraction Trends (2019-2023)'
                    }
                },
                scales: {
                    y: {
                        beginAtZero: true,
                        title: {
                            display: true,
                            text: 'Extraction Rate (%)'
                        }
                    }
                }
            }
        };

        if (this.trendChart) {
            this.trendChart.destroy();
        }
        this.trendChart = new Chart(ctx, config);
    }

    createCategoryChart() {
        const ctx = document.getElementById('categoryChart').getContext('2d');

        const data = {
            labels: ['Safe (<70%)', 'Semi-Critical (70-90%)', 'Critical (90-100%)', 'Over-Exploited (>100%)'],
            datasets: [{
                label: 'Number of Assessment Units',
                data: [1250, 890, 650, 210],
                backgroundColor: [
                    'rgba(40, 167, 69, 0.8)',
                    'rgba(255, 193, 7, 0.8)',
                    'rgba(255, 87, 34, 0.8)',
                    'rgba(244, 67, 54, 0.8)'
                ],
                borderColor: [
                    'rgb(40, 167, 69)',
                    'rgb(255, 193, 7)',
                    'rgb(255, 87, 34)',
                    'rgb(244, 67, 54)'
                ],
                borderWidth: 2
            }]
        };

        const config = {
            type: 'doughnut',
            data: data,
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    title: {
                        display: true,
                        text: 'Groundwater Categories Distribution (India)'
                    },
                    legend: {
                        position: 'bottom'
                    }
                }
            }
        };

        if (this.categoryChart) {
            this.categoryChart.destroy();
        }
        this.categoryChart = new Chart(ctx, config);
    }

    createCorrelationChart() {
        const ctx = document.getElementById('correlationChart').getContext('2d');

        const data = {
            datasets: [{
                label: 'Rainfall vs Extraction',
                data: [
                    { x: 1200, y: 65 }, // Kerala
                    { x: 800, y: 145 },  // Haryana
                    { x: 600, y: 165 },  // Punjab
                    { x: 900, y: 156 },  // Maharashtra
                    { x: 700, y: 167 },  // Tamil Nadu
                    { x: 850, y: 155 },  // Karnataka
                    { x: 650, y: 142 },  // Rajasthan
                    { x: 750, y: 134 }   // Gujarat
                ],
                backgroundColor: 'rgba(54, 162, 235, 0.6)',
                borderColor: 'rgb(54, 162, 235)',
                borderWidth: 2,
                pointRadius: 8,
                pointHoverRadius: 12
            }]
        };

        const config = {
            type: 'scatter',
            data: data,
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    title: {
                        display: true,
                        text: 'Rainfall vs Groundwater Extraction Correlation'
                    },
                    tooltip: {
                        callbacks: {
                            label: function(context) {
                                return `Rainfall: ${context.parsed.x}mm, Extraction: ${context.parsed.y}%`;
                            }
                        }
                    }
                },
                scales: {
                    x: {
                        title: {
                            display: true,
                            text: 'Annual Rainfall (mm)'
                        }
                    },
                    y: {
                        title: {
                            display: true,
                            text: 'Groundwater Extraction (%)'
                        },
                        beginAtZero: true
                    }
                }
            }
        };

        if (this.correlationChart) {
            this.correlationChart.destroy();
        }
        this.correlationChart = new Chart(ctx, config);
    }

    updateAnalytics() {
        // Update summary cards with real data
        document.getElementById('criticalState').textContent = 'Punjab (165% extraction)';
        document.getElementById('safeState').textContent = 'Kerala (65% extraction)';
        document.getElementById('nationalAvg').textContent = '125% extraction rate';
        document.getElementById('prediction').textContent = '+5% increase by 2026';
    }

    updateUILanguage() {
        const translations = {
            en: {
                title: "INGRES ChatBot - Groundwater Assistant",
                placeholder: "Ask me about groundwater data in India...",
                welcome: "🌊 Welcome to INGRES ChatBot!\n\nI'm your AI assistant for groundwater resource information across India. I can help you with:\n\n📊 **Groundwater Data Queries**\n• \"Show me groundwater data for Punjab\"\n• \"What's the status of Maharashtra's water resources?\"\n\n📈 **Historical Analysis**\n• \"Compare groundwater trends for Gujarat vs Rajasthan\"\n• \"Show me 5-year data for Karnataka\"\n\n⚠️ **Critical Area Identification**\n• \"Which areas are over-exploited?\"\n• \"Show me safe groundwater zones\"\n\n❓ **General Help**\n• \"Explain groundwater categories\"\n• \"What does stage of extraction mean?\"\n\nTry asking me anything about India's groundwater resources!"
            },
            hi: {
                title: "INGRES चैटबॉट - भूजल सहायक",
                placeholder: "भारत में भूजल डेटा के बारे में मुझे पूछें...",
                welcome: "🌊 INGRES चैटबॉट में आपका स्वागत है!\n\nमैं भारत भर में भूजल संसाधन जानकारी के लिए आपका AI सहायक हूं। मैं आपकी मदद कर सकता हूं:\n\n📊 **भूजल डेटा क्वेरी**\n• \"पंजाब के लिए भूजल डेटा दिखाएं\"\n• \"महाराष्ट्र के जल संसाधनों की क्या स्थिति है?\"\n\n📈 **ऐतिहासिक विश्लेषण**\n• \"गुजरात बनाम राजस्थान के लिए भूजल प्रवृत्तियों की तुलना करें\"\n• \"कर्नाटक के लिए 5 साल का डेटा दिखाएं\"\n\n⚠️ **महत्वपूर्ण क्षेत्र पहचान**\n• \"कौन से क्षेत्र अति-शोषित हैं?\"\n• \"मुझे सुरक्षित भूजल क्षेत्र दिखाएं\"\n\n❓ **सामान्य सहायता**\n• \"भूजल श्रेणियों की व्याख्या करें\"\n• \"निकासी का स्तर क्या होता है?\"\n\nभारत के भूजल संसाधनों के बारे में कुछ भी पूछें!"
            }
        };

        const lang = translations[this.currentLanguage] || translations.en;

        // Update page title
        document.title = lang.title;

        // Update input placeholder
        this.userInput.placeholder = lang.placeholder;

        // Update welcome message if it exists
        const welcomeMessage = document.querySelector('.message.bot .message-content');
        if (welcomeMessage && welcomeMessage.textContent.includes('Welcome to INGRES')) {
            welcomeMessage.textContent = lang.welcome;
        }
    }

    updateVoiceLanguage() {
        // Update speech recognition language
        if (this.recognition) {
            const langMap = {
                en: 'en-IN',
                hi: 'hi-IN',
                gu: 'gu-IN',
                mr: 'mr-IN',
                ta: 'ta-IN',
                te: 'te-IN',
                bn: 'bn-IN',
                kn: 'kn-IN'
            };
            this.recognition.lang = langMap[this.currentLanguage] || 'en-IN';
        }

        // Update speech synthesis language
        if (this.synth) {
            const voiceLangMap = {
                en: 'en-IN',
                hi: 'hi-IN',
                gu: 'gu-IN',
                mr: 'mr-IN',
                ta: 'ta-IN',
                te: 'te-IN',
                bn: 'bn-IN',
                kn: 'kn-IN'
            };

            // Set speech synthesis voice
            const voices = this.synth.getVoices();
            const preferredVoice = voices.find(voice =>
                voice.lang === voiceLangMap[this.currentLanguage]
            );
            if (preferredVoice) {
                // Voice selection will be applied when speaking
            }
        }
    }

    getLanguageName(code) {
        const names = {
            en: 'English',
            hi: 'हिंदी',
            gu: 'ગુજરાતી',
            mr: 'मराठी',
            ta: 'தமிழ்',
            te: 'తెలుగు',
            bn: 'বাংলা',
            kn: 'ಕನ್ನಡ'
        };
        return names[code] || 'English';
    }

    createComparisonChart(state1, state2) {
        const ctx = document.getElementById('dataChart').getContext('2d');

        const data = {
            labels: ['Annual Recharge', 'Annual Extraction', 'Net Availability'],
            datasets: [{
                label: state1,
                data: [156.8, 178.5, 89.2],
                backgroundColor: 'rgba(255, 99, 132, 0.6)',
                borderColor: 'rgba(255, 99, 132, 1)',
                borderWidth: 2
            }, {
                label: state2,
                data: [138.9, 165.2, 79.5],
                backgroundColor: 'rgba(54, 162, 235, 0.6)',
                borderColor: 'rgba(54, 162, 235, 1)',
                borderWidth: 2
            }]
        };

        const config = {
            type: 'bar',
            data: data,
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    title: {
                        display: true,
                        text: `${state1} vs ${state2} Groundwater Comparison`,
                        font: {
                            size: 16,
                            weight: 'bold'
                        }
                    }
                },
                scales: {
                    y: {
                        beginAtZero: true,
                        title: {
                            display: true,
                            text: 'Million Cubic Meters (MCM)'
                        }
                    }
                }
            }
        };

        if (this.currentChart) {
            this.currentChart.destroy();
        }
        this.currentChart = new Chart(ctx, config);
    }

    createTrendChart() {
        const ctx = document.getElementById('dataChart').getContext('2d');

        const data = {
            labels: ['2017', '2018', '2019', '2020', '2021', '2022', '2023'],
            datasets: [{
                label: 'Punjab Stage of Extraction',
                data: [105.2, 108.7, 111.3, 114.8, 117.2, 119.5, 113.8],
                borderColor: 'rgba(255, 99, 132, 1)',
                backgroundColor: 'rgba(255, 99, 132, 0.1)',
                borderWidth: 3,
                fill: true,
                tension: 0.4
            }]
        };

        const config = {
            type: 'line',
            data: data,
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    title: {
                        display: true,
                        text: 'Groundwater Trend Analysis (2017-2023)',
                        font: {
                            size: 16,
                            weight: 'bold'
                        }
                    }
                },
                scales: {
                    y: {
                        beginAtZero: false,
                        title: {
                            display: true,
                            text: 'Stage of Extraction (%)'
                        }
                    },
                    x: {
                        title: {
                            display: true,
                            text: 'Year'
                        }
                    }
                }
            }
        };

        if (this.currentChart) {
            this.currentChart.destroy();
        }
        this.currentChart = new Chart(ctx, config);
    }
}

// Global functions for quick actions
function sendQuickQuery(query) {
    if (window.chatBot) {
        window.chatBot.sendQuickQuery(query);
    }
}

function sendMessage() {
    if (window.chatBot) {
        window.chatBot.sendMessage();
    }
}

// Visualization functions
function showVisualization() {
    if (window.chatBot) {
        window.chatBot.showVisualization();
    }
}

function closeVisualization() {
    if (window.chatBot) {
        window.chatBot.closeVisualization();
    }
}

function showComparisonChart(state1, state2) {
    if (window.chatBot) {
        window.chatBot.createComparisonChart(state1, state2);
    }
}

function showTrendChart() {
    if (window.chatBot) {
        window.chatBot.createTrendChart();
    }
}

// Voice functions
function toggleVoiceInput() {
    if (window.chatBot) {
        window.chatBot.toggleVoiceInput();
    }
}

function speakResponse(text) {
    if (window.chatBot) {
        window.chatBot.speakText(text);
    }
}

// Language functions
function changeLanguage() {
    if (window.chatBot) {
        window.chatBot.changeLanguage();
    }
}

// Initialize chatbot when page loads
document.addEventListener('DOMContentLoaded', () => {
    window.chatBot = new INGRESChatBot();

    // Check authentication on load
    window.chatBot.updateAuthUI();

    // Periodic connection check
    setInterval(() => {
        window.chatBot.checkConnection();
    }, 30000); // Check every 30 seconds

    // Periodic token validation (every 5 minutes)
    setInterval(() => {
        if (window.chatBot.isAuthenticated()) {
            window.chatBot.validateToken();
        }
    }, 300000);
});

// Handle page visibility changes
document.addEventListener('visibilitychange', () => {
    if (!document.hidden && window.chatBot) {
        window.chatBot.checkConnection();
    }
});