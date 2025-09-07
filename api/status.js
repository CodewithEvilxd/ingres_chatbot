// Vercel Serverless Function for System Status
module.exports = async (req, res) => {
  // Enable CORS
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type');

  if (req.method === 'OPTIONS') {
    res.status(200).end();
    return;
  }

  if (req.method !== 'GET') {
    return res.status(405).json({ error: 'Method not allowed' });
  }

  try {
    const uptime = process.uptime();
    const memoryUsage = process.memoryUsage();

    res.status(200).json({
      status: 'online',
      version: '2.0.0-enhanced',
      intent_count: 70,
      server_time: new Date().toISOString(),
      uptime: Math.floor(uptime),
      memory_usage: {
        rss: Math.round(memoryUsage.rss / 1024 / 1024), // MB
        heapTotal: Math.round(memoryUsage.heapTotal / 1024 / 1024),
        heapUsed: Math.round(memoryUsage.heapUsed / 1024 / 1024),
        external: Math.round(memoryUsage.external / 1024 / 1024)
      },
      environment: process.env.NODE_ENV || 'development',
      platform: 'vercel'
    });

  } catch (error) {
    console.error('Status API Error:', error);
    res.status(500).json({
      error: 'Internal server error',
      status: 'error'
    });
  }
};