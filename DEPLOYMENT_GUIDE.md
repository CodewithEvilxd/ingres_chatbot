# 🚀 INGRES ChatBot - Deployment Guide

## Smart India Hackathon 2025 - Production Deployment

---

## 📋 Quick Start (5 Minutes)

### Prerequisites
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential cmake git postgresql postgresql-contrib
sudo apt-get install libpq-dev libjson-c-dev libcurl4-openssl-dev

# Install Node.js for web interface enhancements (optional)
curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
sudo apt-get install -y nodejs
```

### Build & Deploy
```bash
# Clone repository
git clone https://github.com/your-team/ingres-chatbot.git
cd ingres-chatbot

# Build the system
mkdir build && cd build
cmake ..
make -j$(nproc)

# Start the server
./bin/ingres_chatbot --server --port 8080

# Access web interface
open http://localhost:8080
```

---

## 🏗️ Production Deployment Options

### **Option 1: Docker Deployment (Recommended)**

#### Dockerfile
```dockerfile
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential cmake git \
    libpq-dev libjson-c-dev libcurl4-openssl-dev \
    postgresql-client \
    && rm -rf /var/lib/apt/lists/*

# Copy source code
WORKDIR /app
COPY . .

# Build application
RUN mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc)

# Expose port
EXPOSE 8080

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/api/health || exit 1

# Run application
CMD ["./build/bin/ingres_chatbot", "--server", "--port", "8080"]
```

#### Docker Compose (Full Stack)
```yaml
version: '3.8'
services:
  ingres-chatbot:
    build: .
    ports:
      - "8080:8080"
    environment:
      - DATABASE_URL=postgresql://user:password@db:5432/ingres
    depends_on:
      - db
    restart: unless-stopped

  db:
    image: postgres:15
    environment:
      - POSTGRES_DB=ingres
      - POSTGRES_USER=user
      - POSTGRES_PASSWORD=password
    volumes:
      - postgres_data:/var/lib/postgresql/data
      - ./database_setup.sql:/docker-entrypoint-initdb.d/01-setup.sql
      - ./populate_database.sql:/docker-entrypoint-initdb.d/02-populate.sql
    restart: unless-stopped

volumes:
  postgres_data:
```

#### Deploy Commands
```bash
# Build and run
docker-compose up --build -d

# Check logs
docker-compose logs -f ingres-chatbot

# Scale the service
docker-compose up -d --scale ingres-chatbot=3
```

### **Option 2: AWS Deployment**

#### EC2 + RDS Setup
```bash
# Launch EC2 instance
aws ec2 run-instances \
    --image-id ami-0abcdef1234567890 \
    --instance-type t3.medium \
    --key-name your-key-pair \
    --security-groups ingress-chatbot-sg

# Create RDS PostgreSQL instance
aws rds create-db-instance \
    --db-instance-identifier ingres-db \
    --db-instance-class db.t3.micro \
    --engine postgres \
    --master-username admin \
    --master-user-password your-password \
    --allocated-storage 20
```

#### Application Load Balancer
```bash
# Create ALB
aws elbv2 create-load-balancer \
    --name ingres-chatbot-alb \
    --subnets subnet-12345 subnet-67890 \
    --security-groups sg-12345

# Create target group
aws elbv2 create-target-group \
    --name ingres-chatbot-tg \
    --protocol HTTP \
    --port 8080 \
    --vpc-id vpc-12345
```

### **Option 3: Google Cloud Run**

#### Cloud Build Configuration
```yaml
steps:
  - name: 'gcr.io/cloud-builders/docker'
    args: ['build', '-t', 'gcr.io/$PROJECT_ID/ingres-chatbot', '.']

  - name: 'gcr.io/cloud-builders/docker'
    args: ['push', 'gcr.io/$PROJECT_ID/ingres-chatbot']

  - name: 'gcr.io/google.com/cloudsdktool/cloud-sdk'
    entrypoint: gcloud
    args:
      - run
      - deploy
      - ingres-chatbot
      - --image=gcr.io/$PROJECT_ID/ingres-chatbot
      - --platform=managed
      - --port=8080
      - --memory=1Gi
      - --cpu=1
      - --max-instances=10
      - --concurrency=80
```

#### Deploy to Cloud Run
```bash
# Build and deploy
gcloud run deploy ingres-chatbot \
    --source . \
    --platform managed \
    --port 8080 \
    --memory 1Gi \
    --cpu 1 \
    --max-instances 10 \
    --allow-unauthenticated \
    --set-env-vars DATABASE_URL=$DATABASE_URL
```

---

## 📊 Performance Optimization

### **Memory Optimization**
```c
// In main.c - Memory pool allocation
#define MAX_CONNECTIONS 1000
#define MEMORY_POOL_SIZE (MAX_CONNECTIONS * 1024 * 1024)  // 1GB pool

static char* memory_pool = NULL;

void init_memory_pool() {
    memory_pool = malloc(MEMORY_POOL_SIZE);
    if (!memory_pool) {
        log_message(LOG_ERROR, "Failed to allocate memory pool");
        exit(1);
    }
    log_message(LOG_INFO, "Memory pool initialized: %d MB", MEMORY_POOL_SIZE / (1024*1024));
}
```

### **Connection Pooling**
```c
// Database connection pool
#define MAX_DB_CONNECTIONS 10
static PGconn* db_connections[MAX_DB_CONNECTIONS];

void init_db_pool() {
    for (int i = 0; i < MAX_DB_CONNECTIONS; i++) {
        db_connections[i] = PQconnectdb(connection_string);
        if (PQstatus(db_connections[i]) != CONNECTION_OK) {
            log_message(LOG_ERROR, "Failed to create DB connection %d", i);
        }
    }
    log_message(LOG_INFO, "Database connection pool initialized");
}
```

### **Caching Layer**
```c
// Redis integration for caching
#include <hiredis/hiredis.h>

redisContext* redis_ctx = NULL;

void init_cache() {
    redis_ctx = redisConnect("127.0.0.1", 6379);
    if (redis_ctx->err) {
        log_message(LOG_ERROR, "Redis connection failed: %s", redis_ctx->errstr);
    } else {
        log_message(LOG_INFO, "Redis cache connected");
    }
}

char* get_cached_response(const char* query) {
    redisReply* reply = redisCommand(redis_ctx, "GET %s", query);
    if (reply->type == REDIS_REPLY_STRING) {
        return strdup(reply->str);
    }
    freeReplyObject(reply);
    return NULL;
}
```

---

## 🔧 Monitoring & Logging

### **System Monitoring**
```bash
# Install monitoring tools
sudo apt-get install prometheus-node-exporter grafana

# Application metrics endpoint
curl http://localhost:8080/api/metrics
```

### **Log Analysis**
```bash
# View real-time logs
tail -f ingres_chatbot.log

# Search for errors
grep "ERROR" ingres_chatbot.log | tail -10

# Performance analysis
grep "PERF" ingres_chatbot.log | awk '{print $4}' | sort -n
```

### **Health Checks**
```bash
# API health check
curl http://localhost:8080/api/health

# Database connectivity
curl http://localhost:8080/api/db-status

# System resources
curl http://localhost:8080/api/system-info
```

---

## 🔒 Security Configuration

### **SSL/TLS Setup**
```bash
# Generate SSL certificate
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365

# Start with SSL
./bin/ingres_chatbot --server --port 8443 --ssl-cert cert.pem --ssl-key key.pem
```

### **Rate Limiting**
```c
// Implement rate limiting
typedef struct {
    char ip_address[16];
    int request_count;
    time_t last_request;
} RateLimitEntry;

#define MAX_RATE_LIMIT_ENTRIES 1000
static RateLimitEntry rate_limits[MAX_RATE_LIMIT_ENTRIES];

bool check_rate_limit(const char* ip) {
    // Implementation for rate limiting logic
    return true; // Placeholder
}
```

### **Input Validation**
```c
bool validate_input(const char* input) {
    if (!input || strlen(input) == 0) return false;
    if (strlen(input) > MAX_INPUT_LENGTH) return false;

    // Check for malicious patterns
    if (strstr(input, "<script>") || strstr(input, "javascript:")) {
        return false;
    }

    return true;
}
```

---

## 📈 Scaling Strategies

### **Horizontal Scaling**
```bash
# Load balancer configuration
upstream ingres_backend {
    server 127.0.0.1:8080;
    server 127.0.0.1:8081;
    server 127.0.0.1:8082;
}

server {
    listen 80;
    location / {
        proxy_pass http://ingres_backend;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }
}
```

### **Database Scaling**
```sql
-- Read replica setup
CREATE PUBLICATION ingres_pub FOR ALL TABLES;
CREATE SUBSCRIPTION ingres_sub
    CONNECTION 'host=primary_host dbname=ingres user=replica_user'
    PUBLICATION ingres_pub;

-- Partitioning strategy
CREATE TABLE groundwater_data_y2023 PARTITION OF groundwater_data
    FOR VALUES FROM ('2023-01-01') TO ('2024-01-01');
```

### **CDN Integration**
```bash
# CloudFront distribution for static assets
aws cloudfront create-distribution \
    --origin-domain-name your-domain.com \
    --default-cache-behavior '{"TargetOriginId":"ingres-chatbot","ViewerProtocolPolicy":"redirect-to-https"}'
```

---

## 🧪 Testing & Quality Assurance

### **Automated Testing**
```bash
# Run comprehensive test suite
./bin/test_suite

# Performance testing
ab -n 10000 -c 100 http://localhost:8080/api/chat

# Memory leak detection
valgrind --leak-check=full ./bin/ingres_chatbot --test-mode
```

### **Continuous Integration**
```yaml
# .github/workflows/ci.yml
name: CI/CD Pipeline
on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Install dependencies
        run: sudo apt-get install -y build-essential cmake libpq-dev libjson-c-dev
      - name: Build
        run: mkdir build && cd build && cmake .. && make
      - name: Run tests
        run: cd build && ./bin/test_suite
      - name: Performance test
        run: cd build && ./bin/ingres_chatbot --perf-test

  deploy:
    needs: test
    runs-on: ubuntu-latest
    if: github.ref == 'refs/heads/main'
    steps:
      - name: Deploy to production
        run: ./scripts/deploy_production.sh
```

---

## 📞 Support & Maintenance

### **Log Rotation**
```bash
# Logrotate configuration
cat > /etc/logrotate.d/ingres-chatbot << EOF
/var/log/ingres-chatbot.log {
    daily
    rotate 30
    compress
    missingok
    notifempty
    create 644 root root
    postrotate
        systemctl reload ingres-chatbot
    endscript
}
EOF
```

### **Backup Strategy**
```bash
# Database backup script
#!/bin/bash
DATE=$(date +%Y%m%d_%H%M%S)
pg_dump -h localhost -U admin ingres > backup_$DATE.sql

# Automated backup with cron
echo "0 2 * * * /path/to/backup.sh" | crontab -
```

### **Monitoring Alerts**
```bash
# Prometheus alerting rules
groups:
  - name: ingres-chatbot
    rules:
      - alert: HighResponseTime
        expr: rate(http_request_duration_seconds{quantile="0.5"}[5m]) > 0.5
        for: 5m
        labels:
          severity: warning
        annotations:
          summary: "High response time detected"

      - alert: HighErrorRate
        expr: rate(http_requests_total{status="500"}[5m]) / rate(http_requests_total[5m]) > 0.05
        for: 5m
        labels:
          severity: critical
        annotations:
          summary: "High error rate detected"
```

---

## 🎯 Success Metrics

### **Performance KPIs**
- **Response Time:** <100ms for 95% of queries
- **Uptime:** 99.9% availability
- **Error Rate:** <1% of total requests
- **Concurrent Users:** Support 10,000+ simultaneous connections

### **User Experience KPIs**
- **Query Success Rate:** >95% of queries answered correctly
- **User Satisfaction:** >4.5/5 rating
- **Feature Adoption:** >80% of users use advanced features

### **Business Impact KPIs**
- **Data Accuracy:** >98% alignment with CGWB data
- **Policy Impact:** Integration with 5+ state governments
- **Economic Value:** ₹100 crores+ in water savings

---

## 📞 Emergency Contacts

**Technical Support Team**
- **Lead Developer:** [Your Name] - [email]
- **DevOps Engineer:** [Team Member] - [email]
- **Database Admin:** [Team Member] - [email]

**Emergency Procedures**
1. Check system logs: `tail -f /var/log/ingres-chatbot.log`
2. Verify database connectivity: `psql -h localhost -U admin -d ingres`
3. Restart services: `systemctl restart ingres-chatbot`
4. Contact on-call engineer for critical issues

---

**🚀 Ready for production deployment and national scale impact!**