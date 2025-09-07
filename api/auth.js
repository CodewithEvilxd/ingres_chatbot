// Vercel Serverless Function for Authentication
// JWT-based authentication with role-based access control

const jwt = require('jsonwebtoken');
const crypto = require('crypto');

// JWT Secret (in production, use environment variable)
const JWT_SECRET = process.env.JWT_SECRET || 'ingres-chatbot-secret-key-2025';
const JWT_EXPIRES_IN = '24h';

// User roles
const ROLES = {
  ADMIN: 'admin',
  ANALYST: 'analyst',
  USER: 'user',
  GUEST: 'guest'
};

// Mock user database (in production, use real database)
const users = [
  {
    id: 1,
    username: 'admin',
    password: hashPassword('admin123'),
    role: ROLES.ADMIN,
    name: 'System Administrator'
  },
  {
    id: 2,
    username: 'analyst',
    password: hashPassword('analyst123'),
    role: ROLES.ANALYST,
    name: 'Data Analyst'
  },
  {
    id: 3,
    username: 'demo',
    password: hashPassword('demo123'),
    role: ROLES.USER,
    name: 'Demo User'
  }
];

function hashPassword(password) {
  return crypto.createHash('sha256').update(password).digest('hex');
}

function generateToken(user) {
  return jwt.sign(
    {
      userId: user.id,
      username: user.username,
      role: user.role,
      name: user.name
    },
    JWT_SECRET,
    { expiresIn: JWT_EXPIRES_IN }
  );
}

function verifyToken(token) {
  try {
    return jwt.verify(token, JWT_SECRET);
  } catch (error) {
    return null;
  }
}

function authenticateRequest(req) {
  const authHeader = req.headers.authorization;

  if (!authHeader || !authHeader.startsWith('Bearer ')) {
    return { authenticated: false, error: 'No token provided' };
  }

  const token = authHeader.substring(7);
  const decoded = verifyToken(token);

  if (!decoded) {
    return { authenticated: false, error: 'Invalid token' };
  }

  return {
    authenticated: true,
    user: decoded,
    token: token
  };
}

function checkPermission(user, requiredRole) {
  const roleHierarchy = {
    [ROLES.GUEST]: 0,
    [ROLES.USER]: 1,
    [ROLES.ANALYST]: 2,
    [ROLES.ADMIN]: 3
  };

  return roleHierarchy[user.role] >= roleHierarchy[requiredRole];
}

// Rate limiting storage (in production, use Redis)
const rateLimitStore = new Map();

function checkRateLimit(identifier, maxRequests = 100, windowMs = 15 * 60 * 1000) {
  const now = Date.now();
  const windowStart = now - windowMs;

  if (!rateLimitStore.has(identifier)) {
    rateLimitStore.set(identifier, []);
  }

  const requests = rateLimitStore.get(identifier);

  // Remove old requests outside the window
  const validRequests = requests.filter(timestamp => timestamp > windowStart);

  if (validRequests.length >= maxRequests) {
    return { allowed: false, resetTime: windowStart + windowMs };
  }

  // Add current request
  validRequests.push(now);
  rateLimitStore.set(identifier, validRequests);

  return { allowed: true, remaining: maxRequests - validRequests.length };
}

// Input validation
function validateInput(input, type = 'text') {
  if (!input) return { valid: false, error: 'Input is required' };

  switch (type) {
    case 'username':
      if (input.length < 3 || input.length > 50) {
        return { valid: false, error: 'Username must be 3-50 characters' };
      }
      if (!/^[a-zA-Z0-9_]+$/.test(input)) {
        return { valid: false, error: 'Username contains invalid characters' };
      }
      break;

    case 'password':
      if (input.length < 6) {
        return { valid: false, error: 'Password must be at least 6 characters' };
      }
      break;

    case 'message':
      if (input.length > 1000) {
        return { valid: false, error: 'Message too long (max 1000 characters)' };
      }
      // Check for malicious content
      const maliciousPatterns = [
        /<script/i,
        /javascript:/i,
        /on\w+\s*=/i,
        /<iframe/i,
        /<object/i
      ];

      for (const pattern of maliciousPatterns) {
        if (pattern.test(input)) {
          return { valid: false, error: 'Invalid input content' };
        }
      }
      break;
  }

  return { valid: true };
}

// Main authentication endpoints
module.exports = async (req, res) => {
  // Enable CORS
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization');

  if (req.method === 'OPTIONS') {
    res.status(200).end();
    return;
  }

  try {
    const { action } = req.query;

    switch (action) {
      case 'login':
        return handleLogin(req, res);
      case 'register':
        return handleRegister(req, res);
      case 'verify':
        return handleVerify(req, res);
      case 'logout':
        return handleLogout(req, res);
      default:
        return res.status(400).json({ error: 'Invalid action' });
    }
  } catch (error) {
    console.error('Auth error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
};

async function handleLogin(req, res) {
  if (req.method !== 'POST') {
    return res.status(405).json({ error: 'Method not allowed' });
  }

  const { username, password } = req.body;

  // Validate input
  const usernameValidation = validateInput(username, 'username');
  if (!usernameValidation.valid) {
    return res.status(400).json({ error: usernameValidation.error });
  }

  const passwordValidation = validateInput(password, 'password');
  if (!passwordValidation.valid) {
    return res.status(400).json({ error: passwordValidation.error });
  }

  // Check rate limiting
  const rateLimit = checkRateLimit(`login_${username}`, 5, 15 * 60 * 1000); // 5 attempts per 15 minutes
  if (!rateLimit.allowed) {
    return res.status(429).json({
      error: 'Too many login attempts',
      retryAfter: Math.ceil((rateLimit.resetTime - Date.now()) / 1000)
    });
  }

  // Find user
  const user = users.find(u => u.username === username);
  if (!user || user.password !== hashPassword(password)) {
    return res.status(401).json({ error: 'Invalid credentials' });
  }

  // Generate token
  const token = generateToken(user);

  res.status(200).json({
    success: true,
    token: token,
    user: {
      id: user.id,
      username: user.username,
      name: user.name,
      role: user.role
    },
    expiresIn: JWT_EXPIRES_IN
  });
}

async function handleRegister(req, res) {
  if (req.method !== 'POST') {
    return res.status(405).json({ error: 'Method not allowed' });
  }

  const { username, password, name } = req.body;

  // Validate input
  const usernameValidation = validateInput(username, 'username');
  if (!usernameValidation.valid) {
    return res.status(400).json({ error: usernameValidation.error });
  }

  const passwordValidation = validateInput(password, 'password');
  if (!passwordValidation.valid) {
    return res.status(400).json({ error: passwordValidation.error });
  }

  if (!name || name.length < 2 || name.length > 100) {
    return res.status(400).json({ error: 'Name must be 2-100 characters' });
  }

  // Check if user already exists
  if (users.find(u => u.username === username)) {
    return res.status(409).json({ error: 'Username already exists' });
  }

  // Create new user
  const newUser = {
    id: users.length + 1,
    username: username,
    password: hashPassword(password),
    role: ROLES.USER,
    name: name
  };

  users.push(newUser);

  // Generate token
  const token = generateToken(newUser);

  res.status(201).json({
    success: true,
    token: token,
    user: {
      id: newUser.id,
      username: newUser.username,
      name: newUser.name,
      role: newUser.role
    },
    expiresIn: JWT_EXPIRES_IN
  });
}

async function handleVerify(req, res) {
  const auth = authenticateRequest(req);

  if (!auth.authenticated) {
    return res.status(401).json({ error: auth.error });
  }

  res.status(200).json({
    valid: true,
    user: auth.user
  });
}

async function handleLogout(req, res) {
  // In a stateless JWT system, logout is handled client-side
  // by removing the token from storage
  res.status(200).json({ success: true, message: 'Logged out successfully' });
}

// Export utility functions for use in other API endpoints
module.exports.authenticateRequest = authenticateRequest;
module.exports.checkPermission = checkPermission;
module.exports.checkRateLimit = checkRateLimit;
module.exports.validateInput = validateInput;
module.exports.ROLES = ROLES;