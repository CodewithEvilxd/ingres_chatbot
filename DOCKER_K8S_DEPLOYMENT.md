# 🚀 INGRES ChatBot - Docker & Kubernetes Deployment Guide

## Smart India Hackathon 2025 - Production Deployment

---

## 📋 Quick Start Options

### **Option 1: Docker Compose (Recommended for Development)**
```bash
# Clone and deploy
git clone https://github.com/your-team/ingres-chatbot.git
cd ingres-chatbot

# Start all services
docker-compose up --build

# Access the application
open http://localhost:8080
```

### **Option 2: Kubernetes (Production Ready)**
```bash
# Deploy to Kubernetes cluster
./deploy_k8s.sh

# Or manually apply manifests
kubectl apply -f k8s/
```

### **Option 3: Single Docker Container**
```bash
# Build and run
docker build -t ingres-chatbot .
docker run -p 8080:8080 ingres-chatbot
```

---

## 🐳 Docker Deployment

### **Prerequisites**
```bash
# Install Docker
curl -fsSL https://get.docker.com | sh

# Install Docker Compose
sudo curl -L "https://github.com/docker/compose/releases/download/v2.18.1/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
sudo chmod +x /usr/local/bin/docker-compose
```

### **Environment Configuration**
```bash
# Create environment file
cat > .env << EOF
NODE_ENV=production
DATABASE_URL=postgresql://ingres_user:ingres_password@localhost:5432/ingres_db
JWT_SECRET=your-super-secret-jwt-key-change-in-production
EOF
```

### **Build and Run**
```bash
# Build the application
docker build -t ingres-chatbot:latest .

# Run with environment variables
docker run -d \
  --name ingres-chatbot \
  -p 8080:8080 \
  --env-file .env \
  ingres-chatbot:latest

# Check logs
docker logs -f ingres-chatbot
```

### **Docker Compose Stack**
```bash
# Start complete stack (app + database + monitoring)
docker-compose up -d

# View service status
docker-compose ps

# View logs
docker-compose logs -f

# Scale the application
docker-compose up -d --scale ingres-chatbot=3

# Stop all services
docker-compose down
```

---

## ☸️ Kubernetes Deployment

### **Prerequisites**
```bash
# Install kubectl
curl -LO "https://dl.k8s.io/release/$(curl -L -s https://dl.k8s.io/release/stable.txt)/bin/linux/amd64/kubectl"
sudo install -o root -g root -m 0755 kubectl /usr/local/bin/kubectl

# Install Helm (optional)
curl https://raw.githubusercontent.com/helm/helm/main/scripts/get-helm-3 | bash
```

### **Cluster Setup**
```bash
# For local development (Kind)
kind create cluster --name ingres-cluster

# For production clusters, ensure you have:
# - Kubernetes cluster access
# - kubectl configured
# - Cluster admin permissions
```

### **Automated Deployment**
```bash
# One-command deployment
./deploy_k8s.sh

# This will:
# 1. Create namespace
# 2. Apply secrets
# 3. Deploy PostgreSQL
# 4. Deploy application
# 5. Configure ingress
```

### **Manual Deployment Steps**
```bash
# 1. Create namespace
kubectl create namespace ingres-chatbot

# 2. Apply secrets
kubectl apply -f k8s/secrets.yaml

# 3. Deploy database
kubectl apply -f k8s/postgres.yaml

# 4. Wait for database
kubectl wait --for=condition=available deployment/postgres -n ingres-chatbot

# 5. Deploy application
kubectl apply -f k8s/deployment.yaml

# 6. Check deployment status
kubectl get all -n ingres-chatbot
```

### **Access the Application**
```bash
# Port forward for local access
kubectl port-forward -n ingres-chatbot svc/ingres-chatbot-service 8080:80

# Access via ingress (if configured)
# Visit: https://ingres-chatbot.your-domain.com
```

---

## 📊 Monitoring & Observability

### **Application Metrics**
```bash
# View pod status
kubectl get pods -n ingres-chatbot

# View logs
kubectl logs -f deployment/ingres-chatbot -n ingres-chatbot

# Check resource usage
kubectl top pods -n ingres-chatbot

# View events
kubectl get events -n ingres-chatbot --sort-by=.metadata.creationTimestamp
```

### **Health Checks**
```bash
# Application health
curl http://localhost:8080/api/health

# Database connectivity
kubectl exec -n ingres-chatbot deployment/postgres -- pg_isready -h localhost

# API status
curl http://localhost:8080/api/status
```

### **Scaling**
```bash
# Scale application pods
kubectl scale deployment ingres-chatbot -n ingres-chatbot --replicas=5

# Auto-scaling (if HPA configured)
kubectl autoscale deployment ingres-chatbot -n ingres-chatbot --cpu-percent=70 --min=3 --max=10
```

---

## 🔧 Configuration Management

### **Environment Variables**
```yaml
# k8s/secrets.yaml
apiVersion: v1
kind: Secret
metadata:
  name: ingres-secrets
data:
  jwt-secret: <base64-encoded-secret>
  database-url: <base64-encoded-url>
```

### **ConfigMaps**
```yaml
# Application configuration
apiVersion: v1
kind: ConfigMap
metadata:
  name: ingres-config
data:
  NODE_ENV: "production"
  LOG_LEVEL: "info"
  MAX_CONNECTIONS: "100"
```

### **Resource Limits**
```yaml
# In deployment.yaml
resources:
  requests:
    memory: "256Mi"
    cpu: "250m"
  limits:
    memory: "512Mi"
    cpu: "500m"
```

---

## 🔒 Security Best Practices

### **Network Policies**
```yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: ingres-network-policy
spec:
  podSelector:
    matchLabels:
      app: ingres-chatbot
  policyTypes:
  - Ingress
  - Egress
  ingress:
  - from:
    - podSelector:
        matchLabels:
          app: postgres
    ports:
    - protocol: TCP
      port: 5432
```

### **RBAC Configuration**
```yaml
apiVersion: rbac.authorization.k8s.io/v1
kind: Role
metadata:
  name: ingres-role
rules:
- apiGroups: [""]
  resources: ["pods", "services"]
  verbs: ["get", "list", "watch"]
```

### **TLS/SSL Setup**
```yaml
# cert-manager integration
apiVersion: cert-manager.io/v1
kind: ClusterIssuer
metadata:
  name: letsencrypt-prod
spec:
  acme:
    server: https://acme-v02.api.letsencrypt.org/directory
    email: your-email@example.com
    privateKeySecretRef:
      name: letsencrypt-prod
    solvers:
    - http01:
        ingress:
          class: nginx
```

---

## 📈 Performance Optimization

### **Database Optimization**
```sql
-- Create indexes
CREATE INDEX CONCURRENTLY idx_state_year ON groundwater_data(state, assessment_year);
CREATE INDEX CONCURRENTLY idx_extraction_rate ON groundwater_data(stage_of_extraction);

-- Query optimization
EXPLAIN ANALYZE SELECT * FROM groundwater_data WHERE state = 'Punjab';
```

### **Caching Strategies**
```javascript
// Redis integration for session caching
const redis = require('redis');
const client = redis.createClient({ host: process.env.REDIS_HOST });

// Cache frequently accessed data
app.get('/api/cached-data', async (req, res) => {
  const cacheKey = 'groundwater_summary';
  const cached = await client.get(cacheKey);

  if (cached) {
    return res.json(JSON.parse(cached));
  }

  // Fetch from database
  const data = await fetchGroundwaterSummary();
  await client.setex(cacheKey, 3600, JSON.stringify(data)); // Cache for 1 hour

  res.json(data);
});
```

### **Load Balancing**
```yaml
apiVersion: v1
kind: Service
metadata:
  name: ingres-chatbot-service
spec:
  selector:
    app: ingres-chatbot
  ports:
  - port: 80
    targetPort: 8080
  type: LoadBalancer
  sessionAffinity: ClientIP
```

---

## 🚀 CI/CD Pipeline

### **GitHub Actions**
```yaml
# .github/workflows/deploy.yml
name: Deploy to Kubernetes
on:
  push:
    branches: [main]

jobs:
  deploy:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3

    - name: Configure kubectl
      uses: azure/k8s-set-context@v2
      with:
        kubeconfig: ${{ secrets.KUBE_CONFIG }}

    - name: Deploy to Kubernetes
      run: |
        kubectl apply -f k8s/
        kubectl rollout status deployment/ingres-chatbot -n ingres-chatbot
```

### **ArgoCD (GitOps)**
```yaml
apiVersion: argoproj.io/v1alpha1
kind: Application
metadata:
  name: ingres-chatbot
spec:
  project: default
  source:
    repoURL: https://github.com/your-team/ingres-chatbot
    path: k8s
    targetRevision: HEAD
  destination:
    server: https://kubernetes.default.svc
    namespace: ingres-chatbot
```

---

## 🐛 Troubleshooting

### **Common Issues**

#### **Pod CrashLoopBackOff**
```bash
# Check pod logs
kubectl logs -f pod/ingres-chatbot-xxxxx -n ingres-chatbot

# Check events
kubectl describe pod ingres-chatbot-xxxxx -n ingres-chatbot

# Check resource usage
kubectl top pods -n ingres-chatbot
```

#### **Database Connection Issues**
```bash
# Test database connectivity
kubectl exec -it deployment/postgres -n ingres-chatbot -- psql -U ingres_user -d ingres_db

# Check database logs
kubectl logs -f deployment/postgres -n ingres-chatbot
```

#### **Service Mesh Issues**
```bash
# Check service endpoints
kubectl get endpoints -n ingres-chatbot

# Test service connectivity
kubectl run test-pod --image=busybox --rm -it -- /bin/sh
# Inside pod: wget http://ingres-chatbot-service:80/api/health
```

---

## 📊 Backup & Recovery

### **Database Backup**
```bash
# Create backup job
kubectl create job backup-postgres --from=cronjob/postgres-backup -n ingres-chatbot

# Manual backup
kubectl exec deployment/postgres -n ingres-chatbot -- pg_dump -U ingres_user ingres_db > backup.sql
```

### **Application Backup**
```bash
# Backup ConfigMaps and Secrets
kubectl get configmap,secret -n ingres-chatbot -o yaml > backup-config.yaml

# Backup persistent volumes
kubectl get pvc -n ingres-chatbot
```

### **Disaster Recovery**
```bash
# Restore from backup
kubectl apply -f backup-config.yaml
kubectl apply -f k8s/postgres.yaml
kubectl apply -f k8s/deployment.yaml
```

---

## 🎯 Production Checklist

### **Pre-Deployment**
- [ ] Configure production secrets
- [ ] Set up monitoring and alerting
- [ ] Configure backup strategy
- [ ] Test load balancing
- [ ] Verify SSL certificates
- [ ] Set up log aggregation

### **Post-Deployment**
- [ ] Verify application health
- [ ] Test database connectivity
- [ ] Check monitoring dashboards
- [ ] Validate backup procedures
- [ ] Document runbooks
- [ ] Set up on-call rotation

### **Performance Validation**
- [ ] Response time <200ms
- [ ] Error rate <1%
- [ ] CPU usage <70%
- [ ] Memory usage <80%
- [ ] Database connections <90% of limit

---

## 📞 Support & Resources

### **Documentation**
- [Kubernetes Docs](https://kubernetes.io/docs/)
- [Docker Docs](https://docs.docker.com/)
- [Helm Charts](https://helm.sh/docs/)

### **Community Support**
- [Kubernetes Slack](https://slack.k8s.io/)
- [Docker Forums](https://forums.docker.com/)
- [Stack Overflow](https://stackoverflow.com/questions/tagged/kubernetes)

### **INGRES Resources**
- `DEPLOYMENT_GUIDE.md` - Local deployment
- `VERCEL_DEPLOYMENT.md` - Cloud deployment
- `demo_script.md` - Presentation guide

---

**🚀 Your INGRES ChatBot is now production-ready with full Docker & Kubernetes support!**

**Ready to scale to millions of users and revolutionize India's groundwater management! 🏆🌊**