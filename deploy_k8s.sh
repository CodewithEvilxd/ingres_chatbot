#!/bin/bash

# INGRES ChatBot - Kubernetes Deployment Script
# Smart India Hackathon 2025

echo "🚀 INGRES ChatBot - Kubernetes Deployment"
echo "========================================"

# Check if kubectl is installed
if ! command -v kubectl &> /dev/null; then
    echo "❌ kubectl not found. Please install kubectl first."
    echo "   https://kubernetes.io/docs/tasks/tools/"
    exit 1
fi

# Check if connected to cluster
if ! kubectl cluster-info &> /dev/null; then
    echo "❌ Not connected to Kubernetes cluster."
    echo "   Please configure kubectl to connect to your cluster."
    exit 1
fi

echo "📋 Checking prerequisites..."
echo "Current context: $(kubectl config current-context)"
echo "Namespace: $(kubectl config view --minify --output 'jsonpath={..namespace}')"
echo ""

# Create namespace
echo "📁 Creating namespace..."
kubectl create namespace ingres-chatbot --dry-run=client -o yaml | kubectl apply -f -

# Apply secrets
echo "🔐 Applying secrets..."
kubectl apply -f k8s/secrets.yaml

# Deploy PostgreSQL
echo "🐘 Deploying PostgreSQL database..."
kubectl apply -f k8s/postgres.yaml

# Wait for PostgreSQL to be ready
echo "⏳ Waiting for PostgreSQL to be ready..."
kubectl wait --for=condition=available --timeout=300s deployment/postgres -n ingres-chatbot

# Deploy application
echo "🤖 Deploying INGRES ChatBot..."
kubectl apply -f k8s/deployment.yaml

# Wait for application to be ready
echo "⏳ Waiting for application to be ready..."
kubectl wait --for=condition=available --timeout=300s deployment/ingres-chatbot -n ingres-chatbot

# Get service information
echo ""
echo "✅ Deployment completed successfully!"
echo ""
echo "🌐 Service Information:"
echo "   Internal Service: $(kubectl get svc -n ingres-chatbot -o jsonpath='{.items[0].spec.clusterIP}'):80"
echo ""
echo "📊 Pod Status:"
kubectl get pods -n ingres-chatbot
echo ""
echo "🔍 Service Status:"
kubectl get svc -n ingres-chatbot
echo ""
echo "📈 Check application health:"
echo "   kubectl port-forward -n ingres-chatbot svc/ingres-chatbot-service 8080:80"
echo "   Then visit: http://localhost:8080"
echo ""
echo "📋 Useful commands:"
echo "   View logs: kubectl logs -n ingres-chatbot deployment/ingres-chatbot"
echo "   Scale app: kubectl scale -n ingres-chatbot deployment ingres-chatbot --replicas=5"
echo "   Update app: kubectl rollout restart -n ingres-chatbot deployment/ingres-chatbot"
echo ""
echo "🎯 Ready for production deployment!"