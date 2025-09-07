// Vercel WebSocket support for real-time data synchronization
// This enables real-time updates for groundwater data and analytics

const WebSocket = require('ws');

module.exports = async (req, res) => {
  // WebSocket upgrade handling for Vercel
  if (req.headers.upgrade === 'websocket') {
    const wss = new WebSocket.Server({ noServer: true });

    wss.on('connection', (ws) => {
      console.log('WebSocket client connected');

      // Send welcome message
      ws.send(JSON.stringify({
        type: 'welcome',
        message: 'Connected to INGRES real-time data stream',
        timestamp: new Date().toISOString()
      }));

      // Handle incoming messages
      ws.on('message', (data) => {
        try {
          const message = JSON.parse(data.toString());
          handleWebSocketMessage(ws, message);
        } catch (error) {
          ws.send(JSON.stringify({
            type: 'error',
            message: 'Invalid message format',
            timestamp: new Date().toISOString()
          }));
        }
      });

      // Handle client disconnect
      ws.on('close', () => {
        console.log('WebSocket client disconnected');
      });

      // Send periodic updates (simulated real-time data)
      const updateInterval = setInterval(() => {
        if (ws.readyState === WebSocket.OPEN) {
          sendRealTimeUpdate(ws);
        } else {
          clearInterval(updateInterval);
        }
      }, 30000); // Update every 30 seconds
    });

    // Handle the upgrade
    const { socket, head } = req;
    wss.handleUpgrade(req, socket, head, (ws) => {
      wss.emit('connection', ws, req);
    });
  } else {
    res.status(426).json({ error: 'WebSocket upgrade required' });
  }
};

function handleWebSocketMessage(ws, message) {
  switch (message.type) {
    case 'subscribe':
      // Subscribe to specific data streams
      ws.send(JSON.stringify({
        type: 'subscribed',
        streams: message.streams || ['groundwater_data', 'analytics'],
        timestamp: new Date().toISOString()
      }));
      break;

    case 'unsubscribe':
      // Unsubscribe from data streams
      ws.send(JSON.stringify({
        type: 'unsubscribed',
        timestamp: new Date().toISOString()
      }));
      break;

    case 'ping':
      // Respond to ping with pong
      ws.send(JSON.stringify({
        type: 'pong',
        timestamp: new Date().toISOString()
      }));
      break;

    default:
      ws.send(JSON.stringify({
        type: 'unknown_command',
        message: 'Unknown command type',
        timestamp: new Date().toISOString()
      }));
  }
}

function sendRealTimeUpdate(ws) {
  // Simulate real-time groundwater data updates
  const updates = {
    type: 'data_update',
    timestamp: new Date().toISOString(),
    data: {
      groundwater_levels: {
        punjab: {
          level: Math.random() * 10 + 15, // 15-25 meters
          change: (Math.random() - 0.5) * 0.5, // -0.25 to +0.25
          status: 'critical'
        },
        haryana: {
          level: Math.random() * 8 + 12, // 12-20 meters
          change: (Math.random() - 0.5) * 0.3,
          status: 'semi-critical'
        },
        maharashtra: {
          level: Math.random() * 12 + 10, // 10-22 meters
          change: (Math.random() - 0.5) * 0.4,
          status: 'critical'
        }
      },
      rainfall: {
        current: Math.random() * 50 + 100, // 100-150mm
        forecast: Math.random() * 30 + 80, // 80-110mm
        trend: Math.random() > 0.5 ? 'increasing' : 'decreasing'
      },
      alerts: Math.random() > 0.8 ? [{
        type: 'critical',
        location: 'Punjab',
        message: 'Water table dropping rapidly',
        severity: 'high'
      }] : []
    }
  };

  ws.send(JSON.stringify(updates));
}

// For Vercel, we need to export the handler differently
module.exports.default = module.exports;