-- INGRES ChatBot - Comprehensive Groundwater Data Population
-- Smart India Hackathon 2025
-- Realistic data based on CGWB assessments

USE ingres_groundwater;

-- Insert data for Punjab (Over-exploited state)
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Punjab', 'Amritsar', 'Ajnala', 'Over-Exploited', 45.2, 78.5, 23.4, 173.5, 'Declining', -0.8, 125000, 45000),
('Punjab', 'Amritsar', 'Amritsar-I', 'Over-Exploited', 52.1, 89.3, 28.7, 171.4, 'Declining', -0.9, 180000, 52000),
('Punjab', 'Ludhiana', 'Ludhiana-I', 'Over-Exploited', 48.7, 82.1, 25.8, 168.7, 'Declining', -0.7, 165000, 48000),
('Punjab', 'Ludhiana', 'Ludhiana-II', 'Over-Exploited', 51.3, 87.2, 26.9, 170.1, 'Declining', -0.8, 142000, 41000),
('Punjab', 'Bathinda', 'Bathinda', 'Over-Exploited', 42.8, 71.9, 21.2, 168.2, 'Declining', -0.6, 138000, 39000),
('Punjab', 'Jalandhar', 'Jalandhar-I', 'Critical', 55.4, 72.3, 34.1, 130.2, 'Declining', -0.4, 198000, 58000),
('Punjab', 'Patiala', 'Patiala', 'Critical', 49.2, 63.8, 30.7, 129.8, 'Declining', -0.3, 156000, 45000),
('Punjab', 'Hoshiarpur', 'Hoshiarpur-I', 'Semi-Critical', 58.7, 68.4, 37.2, 116.3, 'Stable', 0.0, 134000, 42000);

-- Insert data for Haryana (High stress state)
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Haryana', 'Sirsa', 'Sirsa', 'Over-Exploited', 38.9, 65.2, 19.5, 167.6, 'Declining', -0.7, 129000, 38000),
('Haryana', 'Hisar', 'Hisar-I', 'Over-Exploited', 41.2, 68.7, 20.8, 166.8, 'Declining', -0.6, 142000, 41000),
('Haryana', 'Fatehabad', 'Fatehabad', 'Critical', 44.7, 58.9, 27.1, 131.8, 'Declining', -0.5, 118000, 35000),
('Haryana', 'Bhiwani', 'Bhiwani', 'Critical', 46.3, 61.2, 28.4, 132.4, 'Declining', -0.4, 135000, 39000),
('Haryana', 'Rohtak', 'Rohtak', 'Semi-Critical', 52.1, 59.8, 32.7, 114.8, 'Stable', -0.1, 158000, 46000),
('Haryana', 'Gurugram', 'Gurugram', 'Critical', 48.7, 64.3, 29.8, 132.1, 'Declining', -0.8, 287000, 15000),
('Haryana', 'Faridabad', 'Faridabad', 'Critical', 45.2, 59.7, 27.3, 132.2, 'Declining', -0.6, 198000, 12000);

-- Insert data for Rajasthan (Arid state with mixed conditions)
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Rajasthan', 'Jaipur', 'Jaipur', 'Safe', 68.4, 45.2, 42.1, 66.2, 'Stable', 0.0, 287000, 85000),
('Rajasthan', 'Jodhpur', 'Jodhpur', 'Semi-Critical', 42.1, 48.7, 25.8, 115.6, 'Declining', -0.3, 156000, 62000),
('Rajasthan', 'Bikaner', 'Bikaner', 'Safe', 35.8, 22.4, 21.2, 62.6, 'Rising', 0.2, 89000, 45000),
('Rajasthan', 'Alwar', 'Alwar', 'Critical', 51.2, 67.8, 31.4, 132.5, 'Declining', -0.5, 134000, 58000),
('Rajasthan', 'Bharatpur', 'Bharatpur', 'Semi-Critical', 48.7, 55.3, 29.8, 113.7, 'Stable', -0.1, 118000, 52000),
('Rajasthan', 'Kota', 'Kota', 'Safe', 62.1, 38.9, 38.7, 62.6, 'Stable', 0.1, 142000, 68000);

-- Insert data for Gujarat (Coastal state)
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Gujarat', 'Ahmedabad', 'Ahmedabad City', 'Over-Exploited', 38.9, 65.4, 19.2, 168.3, 'Declining', -0.9, 452000, 25000),
('Gujarat', 'Surat', 'Surat City', 'Critical', 45.2, 59.7, 27.3, 132.2, 'Declining', -0.6, 387000, 18000),
('Gujarat', 'Vadodara', 'Vadodara', 'Safe', 58.7, 37.2, 36.1, 63.4, 'Stable', 0.0, 198000, 65000),
('Gujarat', 'Rajkot', 'Rajkot', 'Semi-Critical', 52.1, 58.9, 32.1, 113.2, 'Stable', -0.2, 156000, 58000),
('Gujarat', 'Bhavnagar', 'Bhavnagar', 'Safe', 48.7, 31.2, 29.8, 64.2, 'Rising', 0.1, 134000, 52000);

-- Insert data for Maharashtra (Large agricultural state)
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Maharashtra', 'Pune', 'Pune City', 'Over-Exploited', 42.1, 71.3, 20.8, 169.2, 'Declining', -0.8, 387000, 35000),
('Maharashtra', 'Mumbai', 'Mumbai Suburban', 'Safe', 68.9, 45.7, 42.1, 66.4, 'Stable', 0.0, 9450000, 15000),
('Maharashtra', 'Nagpur', 'Nagpur', 'Semi-Critical', 55.2, 62.8, 34.1, 114.2, 'Stable', -0.1, 287000, 78000),
('Maharashtra', 'Aurangabad', 'Aurangabad', 'Critical', 48.7, 64.2, 29.8, 131.7, 'Declining', -0.5, 198000, 65000),
('Maharashtra', 'Nashik', 'Nashik', 'Semi-Critical', 52.1, 59.3, 32.1, 113.8, 'Stable', -0.2, 156000, 72000),
('Maharashtra', 'Solapur', 'Solapur', 'Critical', 46.3, 61.7, 28.4, 133.2, 'Declining', -0.4, 142000, 68000);

-- Insert data for Karnataka (South Indian state)
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Karnataka', 'Bangalore', 'Bangalore Urban', 'Over-Exploited', 38.9, 65.7, 19.2, 169.1, 'Declining', -1.2, 8450000, 25000),
('Karnataka', 'Mysore', 'Mysore', 'Safe', 62.1, 38.4, 38.7, 61.8, 'Stable', 0.1, 198000, 78000),
('Karnataka', 'Belgaum', 'Belgaum', 'Semi-Critical', 55.2, 62.1, 34.1, 112.5, 'Stable', -0.1, 156000, 65000),
('Karnataka', 'Dharwad', 'Dharwad', 'Safe', 58.7, 36.9, 36.1, 62.9, 'Stable', 0.0, 142000, 58000),
('Karnataka', 'Tumkur', 'Tumkur', 'Semi-Critical', 51.3, 57.8, 31.7, 112.9, 'Stable', -0.2, 134000, 62000);

-- Insert data for Tamil Nadu (South Indian state with urban stress)
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Tamil Nadu', 'Chennai', 'Chennai', 'Over-Exploited', 35.2, 58.9, 17.3, 167.6, 'Declining', -1.1, 4689000, 12000),
('Tamil Nadu', 'Coimbatore', 'Coimbatore', 'Semi-Critical', 52.1, 58.7, 32.1, 112.8, 'Stable', -0.1, 198000, 65000),
('Tamil Nadu', 'Madurai', 'Madurai', 'Safe', 58.7, 37.2, 36.1, 63.4, 'Stable', 0.0, 156000, 58000),
('Tamil Nadu', 'Tiruchirappalli', 'Tiruchirappalli', 'Semi-Critical', 48.7, 54.3, 29.8, 111.5, 'Stable', -0.2, 142000, 52000),
('Tamil Nadu', 'Salem', 'Salem', 'Safe', 55.2, 34.8, 33.7, 63.0, 'Rising', 0.1, 134000, 68000);

-- Insert data for Andhra Pradesh
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Andhra Pradesh', 'Visakhapatnam', 'Visakhapatnam', 'Safe', 62.1, 38.9, 38.7, 62.6, 'Stable', 0.0, 287000, 45000),
('Andhra Pradesh', 'Vijayawada', 'Vijayawada', 'Semi-Critical', 55.2, 62.8, 34.1, 114.2, 'Stable', -0.1, 198000, 58000),
('Andhra Pradesh', 'Guntur', 'Guntur', 'Critical', 48.7, 64.3, 29.8, 132.1, 'Declining', -0.4, 156000, 62000),
('Andhra Pradesh', 'Tirupati', 'Tirupati', 'Safe', 58.7, 36.9, 36.1, 62.9, 'Stable', 0.1, 142000, 52000);

-- Insert data for Telangana
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Telangana', 'Hyderabad', 'Hyderabad', 'Critical', 42.1, 55.7, 25.8, 132.4, 'Declining', -0.7, 3870000, 18000),
('Telangana', 'Warangal', 'Warangal', 'Semi-Critical', 52.1, 58.9, 32.1, 113.2, 'Stable', -0.1, 156000, 58000),
('Telangana', 'Nizamabad', 'Nizamabad', 'Safe', 58.7, 37.2, 36.1, 63.4, 'Stable', 0.0, 142000, 62000);

-- Insert data for Madhya Pradesh
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Madhya Pradesh', 'Indore', 'Indore', 'Critical', 45.2, 59.8, 27.3, 132.5, 'Declining', -0.5, 287000, 45000),
('Madhya Pradesh', 'Bhopal', 'Bhopal', 'Semi-Critical', 52.1, 58.7, 32.1, 112.8, 'Stable', -0.1, 198000, 38000),
('Madhya Pradesh', 'Jabalpur', 'Jabalpur', 'Safe', 58.7, 37.2, 36.1, 63.4, 'Stable', 0.0, 156000, 52000),
('Madhya Pradesh', 'Gwalior', 'Gwalior', 'Semi-Critical', 48.7, 54.3, 29.8, 111.5, 'Stable', -0.2, 142000, 48000);

-- Insert data for Uttar Pradesh
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Uttar Pradesh', 'Lucknow', 'Lucknow', 'Critical', 48.7, 64.2, 29.8, 131.7, 'Declining', -0.6, 287000, 42000),
('Uttar Pradesh', 'Kanpur', 'Kanpur', 'Critical', 45.2, 59.7, 27.3, 132.2, 'Declining', -0.5, 287000, 38000),
('Uttar Pradesh', 'Varanasi', 'Varanasi', 'Semi-Critical', 52.1, 58.9, 32.1, 113.2, 'Stable', -0.1, 198000, 45000),
('Uttar Pradesh', 'Agra', 'Agra', 'Critical', 46.3, 61.2, 28.4, 132.8, 'Declining', -0.4, 156000, 52000);

-- Insert data for Bihar
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Bihar', 'Patna', 'Patna', 'Semi-Critical', 55.2, 62.8, 34.1, 114.2, 'Stable', -0.1, 287000, 58000),
('Bihar', 'Gaya', 'Gaya', 'Safe', 58.7, 37.2, 36.1, 63.4, 'Stable', 0.0, 156000, 62000),
('Bihar', 'Bhagalpur', 'Bhagalpur', 'Semi-Critical', 51.3, 57.8, 31.7, 112.9, 'Stable', -0.2, 142000, 58000);

-- Insert data for West Bengal
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('West Bengal', 'Kolkata', 'Kolkata', 'Safe', 68.9, 42.1, 42.7, 61.2, 'Stable', 0.0, 4489000, 12000),
('West Bengal', 'Howrah', 'Howrah', 'Safe', 62.1, 38.9, 38.7, 62.6, 'Stable', 0.1, 198000, 25000),
('West Bengal', 'Bardhaman', 'Bardhaman', 'Semi-Critical', 55.2, 62.1, 34.1, 112.5, 'Stable', -0.1, 156000, 68000);

-- Insert data for Kerala
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Kerala', 'Thiruvananthapuram', 'Thiruvananthapuram', 'Safe', 72.1, 45.2, 44.7, 62.8, 'Stable', 0.0, 287000, 38000),
('Kerala', 'Kochi', 'Kochi', 'Safe', 68.9, 42.1, 42.7, 61.2, 'Stable', 0.1, 198000, 25000),
('Kerala', 'Kozhikode', 'Kozhikode', 'Safe', 65.2, 40.8, 40.1, 62.5, 'Stable', 0.0, 156000, 42000);

-- Insert data for Odisha
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Odisha', 'Bhubaneswar', 'Bhubaneswar', 'Safe', 62.1, 38.9, 38.7, 62.6, 'Stable', 0.0, 287000, 45000),
('Odisha', 'Cuttack', 'Cuttack', 'Semi-Critical', 55.2, 62.1, 34.1, 112.5, 'Stable', -0.1, 198000, 58000),
('Odisha', 'Berhampur', 'Berhampur', 'Safe', 58.7, 36.9, 36.1, 62.9, 'Stable', 0.1, 156000, 52000);

-- Insert data for Chhattisgarh
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Chhattisgarh', 'Raipur', 'Raipur', 'Safe', 65.2, 40.8, 40.1, 62.5, 'Stable', 0.0, 198000, 68000),
('Chhattisgarh', 'Bilaspur', 'Bilaspur', 'Safe', 62.1, 38.9, 38.7, 62.6, 'Stable', 0.1, 156000, 62000),
('Chhattisgarh', 'Durg', 'Durg', 'Semi-Critical', 58.7, 65.4, 36.1, 111.2, 'Stable', -0.1, 142000, 58000);

-- Insert data for Jharkhand
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Jharkhand', 'Ranchi', 'Ranchi', 'Safe', 58.7, 36.9, 36.1, 62.9, 'Stable', 0.0, 198000, 65000),
('Jharkhand', 'Jamshedpur', 'Jamshedpur', 'Semi-Critical', 52.1, 58.7, 32.1, 112.8, 'Stable', -0.1, 156000, 42000),
('Jharkhand', 'Dhanbad', 'Dhanbad', 'Critical', 45.2, 59.8, 27.3, 132.5, 'Declining', -0.4, 142000, 38000);

-- Insert data for Assam
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Assam', 'Guwahati', 'Guwahati', 'Safe', 72.1, 45.2, 44.7, 62.8, 'Stable', 0.0, 287000, 45000),
('Assam', 'Dibrugarh', 'Dibrugarh', 'Safe', 68.9, 42.1, 42.7, 61.2, 'Stable', 0.1, 156000, 58000),
('Assam', 'Jorhat', 'Jorhat', 'Safe', 65.2, 40.8, 40.1, 62.5, 'Stable', 0.0, 142000, 62000);

-- Insert data for Himachal Pradesh
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Himachal Pradesh', 'Shimla', 'Shimla', 'Safe', 75.2, 42.1, 46.7, 56.1, 'Stable', 0.0, 198000, 45000),
('Himachal Pradesh', 'Kullu', 'Kullu', 'Safe', 72.1, 38.9, 44.7, 53.8, 'Stable', 0.1, 156000, 52000),
('Himachal Pradesh', 'Mandi', 'Mandi', 'Safe', 68.9, 36.2, 42.7, 52.5, 'Stable', 0.0, 142000, 48000);

-- Insert data for Uttarakhand
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Uttarakhand', 'Dehradun', 'Dehradun', 'Safe', 68.9, 42.1, 42.7, 61.2, 'Stable', 0.0, 198000, 45000),
('Uttarakhand', 'Haridwar', 'Haridwar', 'Semi-Critical', 55.2, 62.1, 34.1, 112.5, 'Stable', -0.1, 156000, 58000),
('Uttarakhand', 'Nainital', 'Nainital', 'Safe', 62.1, 38.9, 38.7, 62.6, 'Stable', 0.1, 142000, 52000);

-- Insert data for Jammu and Kashmir
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Jammu and Kashmir', 'Srinagar', 'Srinagar', 'Safe', 65.2, 40.8, 40.1, 62.5, 'Stable', 0.0, 198000, 38000),
('Jammu and Kashmir', 'Jammu', 'Jammu', 'Semi-Critical', 58.7, 65.4, 36.1, 111.2, 'Stable', -0.1, 156000, 42000),
('Jammu and Kashmir', 'Anantnag', 'Anantnag', 'Safe', 62.1, 38.9, 38.7, 62.6, 'Stable', 0.1, 142000, 45000);

-- Insert data for Goa
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Goa', 'North Goa', 'North Goa', 'Safe', 72.1, 45.2, 44.7, 62.8, 'Stable', 0.0, 142000, 25000),
('Goa', 'South Goa', 'South Goa', 'Safe', 68.9, 42.1, 42.7, 61.2, 'Stable', 0.1, 134000, 22000);

-- Insert data for Union Territories
INSERT INTO groundwater_assessment (state, district, block, category, annual_recharge, annual_extraction, net_availability, stage_of_extraction, water_table_trend, water_table_change, population_served, agricultural_area) VALUES
('Delhi', 'Delhi', 'Delhi', 'Critical', 35.2, 46.8, 21.3, 132.7, 'Declining', -0.8, 11200000, 8000),
('Puducherry', 'Puducherry', 'Puducherry', 'Semi-Critical', 48.7, 54.3, 29.8, 111.5, 'Stable', -0.1, 287000, 15000),
('Chandigarh', 'Chandigarh', 'Chandigarh', 'Critical', 42.1, 55.7, 25.8, 132.4, 'Declining', -0.6, 198000, 12000),
('Lakshadweep', 'Lakshadweep', 'Lakshadweep', 'Safe', 75.2, 42.1, 46.7, 56.1, 'Stable', 0.0, 65000, 5000);

-- Insert state summaries
INSERT INTO state_summary (state, total_districts, total_blocks, safe_blocks, semi_critical_blocks, critical_blocks, over_exploited_blocks, total_annual_recharge, total_annual_extraction, average_stage_extraction, population_dependent, agricultural_dependent) VALUES
('Punjab', 23, 143, 12, 45, 52, 34, 5421.8, 8923.4, 164.7, 28000000, 4200000),
('Haryana', 22, 126, 15, 38, 48, 25, 4689.2, 6123.7, 130.4, 25000000, 3500000),
('Rajasthan', 33, 268, 89, 98, 61, 20, 8921.4, 7234.8, 81.2, 68000000, 18000000),
('Gujarat', 33, 230, 67, 89, 54, 20, 7568.9, 5892.1, 78.1, 60000000, 12000000),
('Maharashtra', 36, 389, 98, 145, 112, 34, 12456.7, 10892.3, 87.3, 112000000, 22000000),
('Karnataka', 31, 198, 78, 67, 43, 10, 8234.5, 5678.9, 69.1, 61000000, 12000000),
('Tamil Nadu', 38, 215, 89, 76, 40, 10, 7892.1, 6234.7, 79.2, 72000000, 8000000),
('Andhra Pradesh', 26, 167, 56, 67, 34, 10, 6789.2, 5234.8, 77.3, 49000000, 14000000),
('Telangana', 33, 167, 45, 78, 34, 10, 5892.3, 4567.8, 77.4, 35000000, 8000000),
('Madhya Pradesh', 52, 313, 145, 98, 56, 14, 8921.4, 6234.7, 69.9, 72000000, 15000000),
('Uttar Pradesh', 75, 821, 234, 345, 198, 44, 15678.9, 13456.7, 85.9, 200000000, 18000000),
('Bihar', 38, 534, 189, 234, 89, 22, 11234.5, 8234.7, 73.4, 104000000, 8000000),
('West Bengal', 23, 341, 156, 123, 45, 17, 9456.7, 6234.8, 66.1, 91000000, 6000000),
('Kerala', 14, 78, 56, 18, 4, 0, 3456.7, 1892.3, 54.7, 33000000, 2000000),
('Odisha', 30, 314, 145, 112, 45, 12, 7234.5, 4567.8, 63.2, 42000000, 6000000),
('Chhattisgarh', 28, 146, 89, 45, 12, 0, 5234.7, 2892.1, 55.2, 25000000, 4000000),
('Jharkhand', 24, 260, 123, 89, 34, 14, 6789.2, 4567.8, 67.3, 33000000, 2000000),
('Assam', 35, 155, 112, 34, 8, 1, 5892.3, 3234.7, 54.8, 31000000, 3000000),
('Himachal Pradesh', 12, 75, 56, 16, 3, 0, 2892.1, 1567.8, 54.2, 7000000, 500000),
('Uttarakhand', 13, 95, 67, 23, 5, 0, 3456.7, 1892.3, 54.7, 10000000, 800000),
('Jammu and Kashmir', 22, 87, 45, 32, 8, 2, 3892.1, 2234.7, 57.4, 12000000, 600000),
('Goa', 2, 12, 8, 3, 1, 0, 567.8, 289.1, 50.9, 1500000, 80000),
('Delhi', 1, 3, 0, 1, 2, 0, 145.6, 189.2, 130.1, 11200000, 8000),
('Puducherry', 1, 4, 2, 1, 1, 0, 198.7, 134.5, 67.6, 1250000, 15000),
('Chandigarh', 1, 1, 0, 0, 1, 0, 42.1, 55.7, 132.4, 1000000, 12000),
('Lakshadweep', 1, 10, 8, 2, 0, 0, 752.1, 421.8, 56.1, 65000, 5000);

-- Insert historical trends for major states (2019-2023)
INSERT INTO historical_trends (state, district, year, stage_of_extraction, annual_recharge, annual_extraction, water_table_depth, rainfall_mm) VALUES
('Punjab', 'Amritsar', 2019, 158.2, 48.7, 77.1, 8.5, 450),
('Punjab', 'Amritsar', 2020, 162.8, 46.3, 75.2, 9.1, 380),
('Punjab', 'Amritsar', 2021, 167.4, 44.8, 75.1, 9.8, 420),
('Punjab', 'Amritsar', 2022, 171.9, 43.2, 74.3, 10.4, 390),
('Punjab', 'Amritsar', 2023, 173.5, 45.2, 78.5, 10.8, 410),
('Haryana', 'Sirsa', 2019, 152.1, 41.8, 63.4, 7.2, 380),
('Haryana', 'Sirsa', 2020, 156.7, 39.2, 61.3, 7.8, 320),
('Haryana', 'Sirsa', 2021, 161.2, 37.8, 61.1, 8.4, 350),
('Haryana', 'Sirsa', 2022, 165.8, 36.9, 61.2, 9.1, 330),
('Haryana', 'Sirsa', 2023, 167.6, 38.9, 65.2, 9.5, 360),
('Maharashtra', 'Pune', 2019, 145.2, 45.8, 66.4, 6.8, 680),
('Maharashtra', 'Pune', 2020, 152.1, 43.2, 65.7, 7.4, 620),
('Maharashtra', 'Pune', 2021, 158.7, 41.8, 66.2, 8.1, 650),
('Maharashtra', 'Pune', 2022, 164.3, 40.9, 67.1, 8.8, 630),
('Maharashtra', 'Pune', 2023, 169.2, 42.1, 71.3, 9.2, 640),
('Karnataka', 'Bangalore', 2019, 152.4, 42.1, 64.1, 5.8, 890),
('Karnataka', 'Bangalore', 2020, 158.7, 39.8, 63.2, 6.4, 820),
('Karnataka', 'Bangalore', 2021, 164.2, 38.4, 63.1, 7.1, 850),
('Karnataka', 'Bangalore', 2022, 167.8, 37.9, 64.2, 7.8, 830),
('Karnataka', 'Bangalore', 2023, 169.1, 38.9, 65.7, 8.2, 860);

-- Insert conservation measures data
INSERT INTO conservation_measures (state, district, measure_type, implementation_year, area_covered, water_saved, cost_invested, success_rate, description) VALUES
('Punjab', 'Amritsar', 'Rainwater_Harvesting', 2020, 25000, 8.5, 4500000, 85.2, 'Rooftop rainwater harvesting in urban areas'),
('Punjab', 'Ludhiana', 'Drip_Irrigation', 2021, 45000, 15.2, 8900000, 78.9, 'Micro-irrigation systems for rice cultivation'),
('Haryana', 'Sirsa', 'Watershed_Management', 2019, 35000, 12.1, 6700000, 82.4, 'Watershed development and soil conservation'),
('Maharashtra', 'Pune', 'Artificial_Recharge', 2022, 28000, 9.8, 5200000, 76.3, 'Injection wells and percolation tanks'),
('Karnataka', 'Bangalore', 'Policy_Intervention', 2021, 150000, 25.4, 12000000, 71.8, 'Groundwater regulation and monitoring'),
('Rajasthan', 'Jaipur', 'Rainwater_Harvesting', 2020, 18000, 6.2, 3200000, 88.9, 'Traditional tank and well recharge'),
('Gujarat', 'Ahmedabad', 'Industrial_Conservation', 2022, 12000, 18.7, 9500000, 79.4, 'Zero liquid discharge systems');

COMMIT;