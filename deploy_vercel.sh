#!/bin/bash

# INGRES ChatBot - Vercel Deployment Script
# Smart India Hackathon 2025

echo "🚀 INGRES ChatBot - Vercel Deployment Script"
echo "=========================================="

# Check if Vercel CLI is installed
if ! command -v vercel &> /dev/null; then
    echo "❌ Vercel CLI not found. Installing..."
    npm install -g vercel
fi

# Check if user is logged in
if ! vercel whoami &> /dev/null; then
    echo "🔐 Please login to Vercel:"
    vercel login
fi

# Check if package.json exists
if [ ! -f "package.json" ]; then
    echo "❌ package.json not found. Please run this script from the project root."
    exit 1
fi

echo "📦 Installing dependencies..."
npm install

echo "🔧 Building project..."
npm run build

echo "🚀 Deploying to Vercel..."
vercel --prod

echo ""
echo "✅ Deployment complete!"
echo ""
echo "🌐 Your app should be available at the URL shown above"
echo "📊 Check Vercel dashboard for deployment status"
echo "🔍 Test the API endpoints:"
echo "   curl https://your-app.vercel.app/api/status"
echo "   curl -X POST https://your-app.vercel.app/api/chat \\"
echo "     -H 'Content-Type: application/json' \\"
echo "     -d '{\"message\": \"Show me Punjab data\"}'"
echo ""
echo "📚 For detailed deployment guide, see VERCEL_DEPLOYMENT.md"
echo ""
echo "🎯 Ready for Smart India Hackathon 2025!"