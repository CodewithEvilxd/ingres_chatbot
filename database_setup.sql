-- INGRES ChatBot - Comprehensive Groundwater Database
-- Smart India Hackathon 2025

-- Create database
CREATE DATABASE IF NOT EXISTS ingres_groundwater;
USE ingres_groundwater;

-- Main groundwater assessment table
CREATE TABLE IF NOT EXISTS groundwater_assessment (
    id SERIAL PRIMARY KEY,
    state VARCHAR(50) NOT NULL,
    district VARCHAR(50) NOT NULL,
    block VARCHAR(50),
    assessment_year INT NOT NULL DEFAULT 2023,
    category ENUM('Safe', 'Semi-Critical', 'Critical', 'Over-Exploited') NOT NULL,
    annual_recharge DECIMAL(10,2), -- Million Cubic Meters
    annual_extraction DECIMAL(10,2), -- Million Cubic Meters
    net_availability DECIMAL(10,2), -- Million Cubic Meters
    stage_of_extraction DECIMAL(5,2), -- Percentage
    water_table_trend ENUM('Rising', 'Stable', 'Declining') DEFAULT 'Declining',
    water_table_change DECIMAL(5,2), -- Meters per year
    population_served INT,
    agricultural_area DECIMAL(10,2), -- Hectares
    industrial_usage DECIMAL(10,2), -- MCM
    domestic_usage DECIMAL(10,2), -- MCM
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    UNIQUE KEY unique_location (state, district, block, assessment_year)
);

-- State-wise summary table
CREATE TABLE IF NOT EXISTS state_summary (
    id SERIAL PRIMARY KEY,
    state VARCHAR(50) NOT NULL UNIQUE,
    total_districts INT,
    total_blocks INT,
    safe_blocks INT DEFAULT 0,
    semi_critical_blocks INT DEFAULT 0,
    critical_blocks INT DEFAULT 0,
    over_exploited_blocks INT DEFAULT 0,
    total_annual_recharge DECIMAL(12,2),
    total_annual_extraction DECIMAL(12,2),
    average_stage_extraction DECIMAL(5,2),
    population_dependent INT,
    agricultural_dependent INT,
    last_updated TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Historical trends table
CREATE TABLE IF NOT EXISTS historical_trends (
    id SERIAL PRIMARY KEY,
    state VARCHAR(50) NOT NULL,
    district VARCHAR(50),
    year INT NOT NULL,
    stage_of_extraction DECIMAL(5,2),
    annual_recharge DECIMAL(10,2),
    annual_extraction DECIMAL(10,2),
    water_table_depth DECIMAL(5,2),
    rainfall_mm INT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_location_year (state, district, year)
);

-- Conservation measures table
CREATE TABLE IF NOT EXISTS conservation_measures (
    id SERIAL PRIMARY KEY,
    state VARCHAR(50) NOT NULL,
    district VARCHAR(50),
    measure_type ENUM('Rainwater_Harvesting', 'Drip_Irrigation', 'Watershed_Management', 'Artificial_Recharge', 'Policy_Intervention'),
    implementation_year INT,
    area_covered DECIMAL(10,2), -- Hectares
    water_saved DECIMAL(10,2), -- MCM
    cost_invested DECIMAL(12,2), -- Rupees
    success_rate DECIMAL(5,2), -- Percentage
    description TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Create indexes for performance
CREATE INDEX idx_state_district ON groundwater_assessment(state, district);
CREATE INDEX idx_category ON groundwater_assessment(category);
CREATE INDEX idx_stage_extraction ON groundwater_assessment(stage_of_extraction);
CREATE INDEX idx_assessment_year ON groundwater_assessment(assessment_year);

-- Insert comprehensive data for all Indian states
-- This will be populated with realistic CGWB data