#include "SystemScorer.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

// ========================================
// Конструктор та деструктор
// ========================================

SystemScorer::SystemScorer() {
    initializeGPUDatabase();
}

SystemScorer::~SystemScorer() {
}

// ========================================
// Ініціалізація бази даних GPU
// ========================================

void SystemScorer::initializeGPUDatabase() {
    // NVIDIA RTX 40 series
    gpu_database_.push_back(GPUBenchmark("RTX 4090", 120.0, 0.50, 1));
    gpu_database_.push_back(GPUBenchmark("RTX 4080", 100.0, 0.45, 1));
    gpu_database_.push_back(GPUBenchmark("RTX 4070 Ti", 85.0, 0.43, 1));
    gpu_database_.push_back(GPUBenchmark("RTX 4070", 75.0, 0.42, 2));
    gpu_database_.push_back(GPUBenchmark("RTX 4060 Ti", 60.0, 0.40, 2));
    gpu_database_.push_back(GPUBenchmark("RTX 4060", 50.0, 0.38, 3));
    
    // NVIDIA RTX 30 series
    gpu_database_.push_back(GPUBenchmark("RTX 3090 Ti", 120.0, 0.36, 1));
    gpu_database_.push_back(GPUBenchmark("RTX 3090", 115.0, 0.35, 2));
    gpu_database_.push_back(GPUBenchmark("RTX 3080 Ti", 105.0, 0.37, 2));
    gpu_database_.push_back(GPUBenchmark("RTX 3080", 95.0, 0.38, 2));
    gpu_database_.push_back(GPUBenchmark("RTX 3070 Ti", 80.0, 0.38, 2));
    gpu_database_.push_back(GPUBenchmark("RTX 3070", 75.0, 0.40, 2));
    gpu_database_.push_back(GPUBenchmark("RTX 3060 Ti", 60.0, 0.42, 3));
    gpu_database_.push_back(GPUBenchmark("RTX 3060", 48.0, 0.40, 3));
    gpu_database_.push_back(GPUBenchmark("RTX 3050", 35.0, 0.35, 4));
    
    // NVIDIA GTX 16 series
    gpu_database_.push_back(GPUBenchmark("GTX 1660 Ti", 30.0, 0.28, 4));
    gpu_database_.push_back(GPUBenchmark("GTX 1660 Super", 28.0, 0.27, 4));
    gpu_database_.push_back(GPUBenchmark("GTX 1660", 26.0, 0.26, 4));
    gpu_database_.push_back(GPUBenchmark("GTX 1650 Super", 18.0, 0.22, 4));
    gpu_database_.push_back(GPUBenchmark("GTX 1650", 15.0, 0.20, 5));
    
    // AMD RX 7000 series
    gpu_database_.push_back(GPUBenchmark("RX 7900 XTX", 100.0, 0.40, 1));
    gpu_database_.push_back(GPUBenchmark("RX 7900 XT", 90.0, 0.38, 2));
    gpu_database_.push_back(GPUBenchmark("RX 7800 XT", 75.0, 0.37, 2));
    gpu_database_.push_back(GPUBenchmark("RX 7700 XT", 65.0, 0.36, 2));
    gpu_database_.push_back(GPUBenchmark("RX 7600 XT", 50.0, 0.35, 3));
    gpu_database_.push_back(GPUBenchmark("RX 7600", 45.0, 0.34, 3));
    
    // AMD RX 6000 series
    gpu_database_.push_back(GPUBenchmark("RX 6950 XT", 70.0, 0.30, 2));
    gpu_database_.push_back(GPUBenchmark("RX 6900 XT", 64.0, 0.28, 2));
    gpu_database_.push_back(GPUBenchmark("RX 6800 XT", 62.0, 0.30, 2));
    gpu_database_.push_back(GPUBenchmark("RX 6800", 58.0, 0.31, 2));
    gpu_database_.push_back(GPUBenchmark("RX 6750 XT", 50.0, 0.32, 3));
    gpu_database_.push_back(GPUBenchmark("RX 6700 XT", 47.0, 0.33, 3));
    gpu_database_.push_back(GPUBenchmark("RX 6650 XT", 35.0, 0.48, 3));
    gpu_database_.push_back(GPUBenchmark("RX 6600 XT", 32.0, 0.48, 3));  // Ваша карта!
    gpu_database_.push_back(GPUBenchmark("RX 6600", 28.0, 0.46, 3));
    gpu_database_.push_back(GPUBenchmark("RX 6500 XT", 18.0, 0.35, 4));
    gpu_database_.push_back(GPUBenchmark("RX 6400", 12.0, 0.30, 5));
    
    // AMD RX 5000 series
    gpu_database_.push_back(GPUBenchmark("RX 5700 XT", 54.0, 0.35, 3));
    gpu_database_.push_back(GPUBenchmark("RX 5700", 50.0, 0.34, 3));
    gpu_database_.push_back(GPUBenchmark("RX 5600 XT", 40.0, 0.33, 3));
    gpu_database_.push_back(GPUBenchmark("RX 5500 XT", 28.0, 0.30, 4));
    
    // Intel Arc
    gpu_database_.push_back(GPUBenchmark("Arc A770", 60.0, 0.35, 3));
    gpu_database_.push_back(GPUBenchmark("Arc A750", 50.0, 0.34, 3));
    gpu_database_.push_back(GPUBenchmark("Arc A580", 40.0, 0.32, 3));
}

// ========================================
// Преднастроєні ваги
// ========================================

TaskWeights SystemScorer::getMiningWeights() {
    return TaskWeights(0.50, 0.25, 0.10, 0.10, 0.05);
}

TaskWeights SystemScorer::getAITrainingWeights() {
    return TaskWeights(0.35, 0.40, 0.10, 0.10, 0.05);
}

TaskWeights SystemScorer::getVideoRenderingWeights() {
    return TaskWeights(0.30, 0.20, 0.30, 0.15, 0.05);
}

TaskWeights SystemScorer::getGamingWeights() {
    return TaskWeights(0.40, 0.25, 0.20, 0.10, 0.05);
}

TaskWeights SystemScorer::getGeneralWeights() {
    return TaskWeights(0.30, 0.20, 0.25, 0.15, 0.10);
}

// ========================================
// Пошук GPU в базі даних
// ========================================

std::optional<GPUBenchmark> SystemScorer::findGPU(const std::string& model) const {
    std::string model_upper = model;
    std::transform(model_upper.begin(), model_upper.end(), model_upper.begin(), ::toupper);
    
    for (const auto& gpu : gpu_database_) {
        std::string pattern_upper = gpu.model_pattern;
        std::transform(pattern_upper.begin(), pattern_upper.end(), pattern_upper.begin(), ::toupper);
        
        if (model_upper.find(pattern_upper) != std::string::npos) {
            return gpu;
        }
    }
    
    return std::nullopt;
}

// ========================================
// Оцінка GPU
// ========================================

double SystemScorer::calculateGPUScore(const std::string& gpu_model, std::string& details) const {
    auto gpu_opt = findGPU(gpu_model);
    
    if (gpu_opt.has_value()) {
        const GPUBenchmark& gpu = gpu_opt.value();
        
        // Нормалізуємо до 0-100 (RTX 4090 = 100)
        double score = (gpu.hashrate / 120.0) * 100.0;
        score = std::min(100.0, score);
        
        std::ostringstream oss;
        oss << "Found in database: " << gpu.model_pattern << "\n"
            << "      Hashrate: " << std::fixed << std::setprecision(1) << gpu.hashrate << " MH/s\n"
            << "      Efficiency: " << std::fixed << std::setprecision(2) << gpu.power_efficiency << " MH/W\n"
            << "      Tier: " << gpu.tier << "/5";
        details = oss.str();
        
        return score;
    }
    
    // GPU не знайдено - повертаємо середній бал
    details = "GPU not found in database - using default score";
    return 50.0;
}

// ========================================
// Оцінка VRAM
// ========================================

double SystemScorer::calculateVRAMScore(uint64_t vram_mb, const std::string& task_type, std::string& details) const {
    double score = 0.0;
    std::ostringstream oss;
    
    if (task_type == "mining") {
        if (vram_mb >= 12000) {
            score = 100.0;
            oss << "12+ GB - Excellent for mining (future-proof)";
        } else if (vram_mb >= 8000) {
            score = 90.0;
            oss << "8+ GB - Great for mining (current requirements)";
        } else if (vram_mb >= 6000) {
            score = 70.0;
            oss << "6+ GB - Good for mining (meets minimum)";
        } else if (vram_mb >= 4000) {
            score = 40.0;
            oss << "4+ GB - Marginal for mining (may struggle)";
        } else {
            score = 20.0;
            oss << "< 4 GB - Insufficient for mining";
        }
    } else if (task_type == "ai_training") {
        if (vram_mb >= 24000) {
            score = 100.0;
            oss << "24+ GB - Excellent for large AI models";
        } else if (vram_mb >= 16000) {
            score = 80.0;
            oss << "16+ GB - Good for medium AI models";
        } else if (vram_mb >= 8000) {
            score = 50.0;
            oss << "8+ GB - Suitable for small AI models";
        } else if (vram_mb >= 4000) {
            score = 25.0;
            oss << "4+ GB - Very limited AI training capability";
        } else {
            score = 10.0;
            oss << "< 4 GB - Insufficient for AI training";
        }
    } else if (task_type == "video_rendering") {
        if (vram_mb >= 16000) {
            score = 100.0;
            oss << "16+ GB - Excellent for 4K/8K rendering";
        } else if (vram_mb >= 8000) {
            score = 85.0;
            oss << "8+ GB - Great for 1080p/4K rendering";
        } else if (vram_mb >= 6000) {
            score = 70.0;
            oss << "6+ GB - Good for 1080p rendering";
        } else if (vram_mb >= 4000) {
            score = 50.0;
            oss << "4+ GB - Suitable for basic rendering";
        } else {
            score = 30.0;
            oss << "< 4 GB - Limited rendering capability";
        }
    } else if (task_type == "gaming") {
        if (vram_mb >= 12000) {
            score = 100.0;
            oss << "12+ GB - Excellent for 4K gaming";
        } else if (vram_mb >= 8000) {
            score = 90.0;
            oss << "8+ GB - Great for 1440p/4K gaming";
        } else if (vram_mb >= 6000) {
            score = 75.0;
            oss << "6+ GB - Good for 1080p/1440p gaming";
        } else if (vram_mb >= 4000) {
            score = 55.0;
            oss << "4+ GB - Suitable for 1080p gaming";
        } else {
            score = 30.0;
            oss << "< 4 GB - Limited gaming capability";
        }
    } else {
        // За замовчуванням - лінійна шкала
        score = std::min(100.0, (vram_mb / 80.0));
        oss << vram_mb << " MB VRAM available";
    }
    
    details = oss.str();
    return score;
}

// Продовження у наступному повідомленні...

// ========================================
// Оцінка CPU
// ========================================

double SystemScorer::calculateCPUScore(uint32_t cores, uint32_t frequency_mhz, std::string& details) const {
    double core_score = 0.0;
    std::string core_rating;

    if (cores >= 16) {
        core_score = 100.0;
        core_rating = "Excellent (16+ cores)";
    }
    else if (cores >= 12) {
        core_score = 85.0;
        core_rating = "Great (12-15 cores)";
    }
    else if (cores >= 8) {
        core_score = 70.0;
        core_rating = "Good (8-11 cores)";
    }
    else if (cores >= 6) {
        core_score = 55.0;
        core_rating = "Adequate (6-7 cores)";
    }
    else if (cores >= 4) {
        core_score = 40.0;
        core_rating = "Minimal (4-5 cores)";
    }
    else {
        core_score = 25.0;
        core_rating = "Insufficient (< 4 cores)";
    }

    // Бонус за частоту
    double frequency_bonus = 0.0;
    std::string freq_rating;

    if (frequency_mhz >= 4000) {
        frequency_bonus = 10.0;
        freq_rating = "Excellent (4.0+ GHz)";
    }
    else if (frequency_mhz >= 3500) {
        frequency_bonus = 7.0;
        freq_rating = "Great (3.5-4.0 GHz)";
    }
    else if (frequency_mhz >= 3000) {
        frequency_bonus = 5.0;
        freq_rating = "Good (3.0-3.5 GHz)";
    }
    else if (frequency_mhz >= 2500) {
        frequency_bonus = 3.0;
        freq_rating = "Adequate (2.5-3.0 GHz)";
    }
    else if (frequency_mhz > 0) {
        frequency_bonus = 1.0;
        freq_rating = "Low (< 2.5 GHz)";
    }
    else {
        freq_rating = "Unknown";
    }

    double total_score = std::min(100.0, core_score + frequency_bonus);

    std::ostringstream oss;
    oss << core_rating << "\n"
        << "      Frequency: " << freq_rating;
    if (frequency_bonus > 0) {
        oss << " (+" << std::fixed << std::setprecision(0) << frequency_bonus << " bonus)";
    }

    details = oss.str();
    return total_score;
}

// ========================================
// Оцінка RAM
// ========================================

double SystemScorer::calculateRAMScore(uint64_t ram_mb, const std::string& task_type, std::string& details) const {
    double score = 0.0;
    std::ostringstream oss;

    if (task_type == "mining") {
        if (ram_mb >= 16000) {
            score = 100.0;
            oss << "16+ GB - Excellent (more than needed)";
        }
        else if (ram_mb >= 8000) {
            score = 100.0;
            oss << "8+ GB - Perfect for mining";
        }
        else if (ram_mb >= 4000) {
            score = 70.0;
            oss << "4+ GB - Adequate for mining";
        }
        else {
            score = 40.0;
            oss << "< 4 GB - May cause issues";
        }
    }
    else if (task_type == "ai_training") {
        if (ram_mb >= 64000) {
            score = 100.0;
            oss << "64+ GB - Excellent for large models";
        }
        else if (ram_mb >= 32000) {
            score = 85.0;
            oss << "32+ GB - Great for medium models";
        }
        else if (ram_mb >= 16000) {
            score = 65.0;
            oss << "16+ GB - Good for small models";
        }
        else if (ram_mb >= 8000) {
            score = 40.0;
            oss << "8+ GB - Limited capability";
        }
        else {
            score = 20.0;
            oss << "< 8 GB - Insufficient";
        }
    }
    else if (task_type == "video_rendering") {
        if (ram_mb >= 64000) {
            score = 100.0;
            oss << "64+ GB - Excellent for professional work";
        }
        else if (ram_mb >= 32000) {
            score = 90.0;
            oss << "32+ GB - Great for 4K editing";
        }
        else if (ram_mb >= 16000) {
            score = 75.0;
            oss << "16+ GB - Good for 1080p/4K editing";
        }
        else if (ram_mb >= 8000) {
            score = 50.0;
            oss << "8+ GB - Adequate for 1080p editing";
        }
        else {
            score = 30.0;
            oss << "< 8 GB - Limited capability";
        }
    }
    else if (task_type == "gaming") {
        if (ram_mb >= 32000) {
            score = 100.0;
            oss << "32+ GB - Excellent (future-proof)";
        }
        else if (ram_mb >= 16000) {
            score = 90.0;
            oss << "16+ GB - Great for modern games";
        }
        else if (ram_mb >= 8000) {
            score = 70.0;
            oss << "8+ GB - Good for most games";
        }
        else {
            score = 40.0;
            oss << "< 8 GB - May struggle with modern games";
        }
    }
    else {
        // За замовчуванням
        if (ram_mb >= 32000) {
            score = 100.0;
            oss << "32+ GB - Excellent";
        }
        else if (ram_mb >= 16000) {
            score = 85.0;
            oss << "16+ GB - Great";
        }
        else if (ram_mb >= 8000) {
            score = 65.0;
            oss << "8+ GB - Good";
        }
        else {
            score = (ram_mb / 80.0);
            oss << ram_mb << " MB - Limited";
        }
    }

    details = oss.str();
    return score;
}

// ========================================
// Оцінка Диску
// ========================================

double SystemScorer::calculateDiskScore(DiskType primary_type, uint64_t free_mb, std::string& details) const {
    double base_score = 0.0;
    std::string type_rating;

    switch (primary_type) {
    case DiskType::SSD:
        base_score = 100.0;
        type_rating = "SSD - Excellent performance";
        break;
    case DiskType::HDD:
        base_score = 50.0;
        type_rating = "HDD - Slower performance";
        break;
    case DiskType::External:
        base_score = 30.0;
        type_rating = "External - May have bandwidth issues";
        break;
    case DiskType::Removable:
        base_score = 20.0;
        type_rating = "Removable - Not recommended";
        break;
    default:
        base_score = 60.0;
        type_rating = "Unknown type";
    }

    // Штраф за мало вільного місця
    std::string space_warning = "";
    double space_multiplier = 1.0;

    uint64_t free_gb = free_mb / 1024;

    if (free_gb < 50) {
        space_multiplier = 0.7;
        space_warning = " (Warning: < 50 GB free - 30% penalty)";
    }
    else if (free_gb < 100) {
        space_multiplier = 0.85;
        space_warning = " (Warning: < 100 GB free - 15% penalty)";
    }
    else if (free_gb < 200) {
        space_multiplier = 0.95;
        space_warning = " (Caution: < 200 GB free - 5% penalty)";
    }
    else {
        space_warning = " (Good: " + std::to_string(free_gb) + " GB free)";
    }

    double final_score = base_score * space_multiplier;

    std::ostringstream oss;
    oss << type_rating << space_warning;
    details = oss.str();

    return final_score;
}

// ========================================
// Визначення рейтингу
// ========================================

std::string SystemScorer::getRatingFromScore(double score) const {
    if (score >= 85) return "Excellent";
    if (score >= 70) return "Good";
    if (score >= 50) return "Average";
    if (score >= 30) return "Poor";
    return "Very Poor";
}

std::string SystemScorer::getRecommendationFromScore(double score, const std::string& task_type) const {
    if (score >= 85) {
        return "System is excellent for " + task_type + "! No upgrades needed.";
    }
    else if (score >= 70) {
        return "System is well-suited for " + task_type + ". Minor upgrades may improve performance.";
    }
    else if (score >= 50) {
        return "System can handle " + task_type + ", but may struggle with demanding workloads. Consider upgrades.";
    }
    else if (score >= 30) {
        return "System is not ideal for " + task_type + ". Significant upgrades recommended.";
    }
    else {
        return "System is not suitable for " + task_type + ". Major upgrades or new system required.";
    }
}

// ========================================
// ГОЛОВНИЙ МЕТОД - Оцінка системи
// ========================================

SystemScore SystemScorer::scoreSystem(
    const ArgentumDevice& device,
    const TaskWeights& weights,
    const std::string& task_type
) const {
    SystemScore score;

    // 1. GPU Score
    if (!device.gpus.empty()) {
        score.gpu_score = calculateGPUScore(device.gpus[0].model, score.gpu_details);
    }
    else {
        score.gpu_score = 0.0;
        score.gpu_details = "No GPU detected";
    }

    // 2. VRAM Score
    if (!device.gpus.empty() && device.gpus[0].vram_mb.has_value()) {
        score.vram_score = calculateVRAMScore(
            device.gpus[0].vram_mb.value(),
            task_type,
            score.vram_details
        );
    }
    else {
        score.vram_score = 0.0;
        score.vram_details = "VRAM information not available";
    }

    // 3. CPU Score
    score.cpu_score = calculateCPUScore(
        device.cpu_cores,
        device.cpu_frequency_mhz.value_or(0),
        score.cpu_details
    );

    // 4. RAM Score
    score.ram_score = calculateRAMScore(device.ram_mb, task_type, score.ram_details);

    // 5. Disk Score
    score.disk_score = calculateDiskScore(
        device.primary_disk_type,
        device.free_disk_mb.value_or(0),
        score.disk_details
    );

    // 6. Зважена загальна оцінка
    score.total_score =
        score.gpu_score * weights.gpu_weight +
        score.vram_score * weights.vram_weight +
        score.cpu_score * weights.cpu_weight +
        score.ram_score * weights.ram_weight +
        score.disk_score * weights.disk_weight;

    // 7. Рейтинг та рекомендації
    score.rating = getRatingFromScore(score.total_score);
    score.recommendation = getRecommendationFromScore(score.total_score, task_type);

    return score;
}

// ========================================
// Вивід результатів
// ========================================

void SystemScorer::printScore(const SystemScore& score) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  SYSTEM PERFORMANCE SCORE" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    std::cout << "Total Score: " << std::fixed << std::setprecision(1)
        << score.total_score << "/100" << std::endl;
    std::cout << "Rating: " << score.rating << std::endl;
    std::cout << std::endl;

    std::cout << "Component Scores:" << std::endl;
    std::cout << "  GPU:  " << std::fixed << std::setprecision(1)
        << std::setw(5) << score.gpu_score << "/100" << std::endl;
    std::cout << "  VRAM: " << std::fixed << std::setprecision(1)
        << std::setw(5) << score.vram_score << "/100" << std::endl;
    std::cout << "  CPU:  " << std::fixed << std::setprecision(1)
        << std::setw(5) << score.cpu_score << "/100" << std::endl;
    std::cout << "  RAM:  " << std::fixed << std::setprecision(1)
        << std::setw(5) << score.ram_score << "/100" << std::endl;
    std::cout << "  Disk: " << std::fixed << std::setprecision(1)
        << std::setw(5) << score.disk_score << "/100" << std::endl;
    std::cout << std::endl;

    std::cout << "Recommendation:" << std::endl;
    std::cout << "  " << score.recommendation << std::endl;
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
}

void SystemScorer::printDetailedScore(const SystemScore& score) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  DETAILED SYSTEM PERFORMANCE SCORE" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    std::cout << "Total Score: " << std::fixed << std::setprecision(1)
        << score.total_score << "/100 (" << score.rating << ")" << std::endl;
    std::cout << std::endl;

    // GPU
    std::cout << "GPU Score: " << std::fixed << std::setprecision(1)
        << score.gpu_score << "/100" << std::endl;
    std::cout << "  Details: " << score.gpu_details << std::endl;
    std::cout << std::endl;

    // VRAM
    std::cout << "VRAM Score: " << std::fixed << std::setprecision(1)
        << score.vram_score << "/100" << std::endl;
    std::cout << "  Details: " << score.vram_details << std::endl;
    std::cout << std::endl;

    // CPU
    std::cout << "CPU Score: " << std::fixed << std::setprecision(1)
        << score.cpu_score << "/100" << std::endl;
    std::cout << "  Details: " << score.cpu_details << std::endl;
    std::cout << std::endl;

    // RAM
    std::cout << "RAM Score: " << std::fixed << std::setprecision(1)
        << score.ram_score << "/100" << std::endl;
    std::cout << "  Details: " << score.ram_details << std::endl;
    std::cout << std::endl;

    // Disk
    std::cout << "Disk Score: " << std::fixed << std::setprecision(1)
        << score.disk_score << "/100" << std::endl;
    std::cout << "  Details: " << score.disk_details << std::endl;
    std::cout << std::endl;

    std::cout << "Overall Recommendation:" << std::endl;
    std::cout << "  " << score.recommendation << std::endl;
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
}

void SystemScorer::printComparison(
    const SystemScore& score1,
    const SystemScore& score2,
    const std::string& name1,
    const std::string& name2
) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  SYSTEM COMPARISON" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    std::cout << std::left << std::setw(20) << "Component"
        << std::right << std::setw(15) << name1
        << std::setw(15) << name2
        << std::setw(15) << "Winner" << std::endl;
    std::cout << std::string(65, '-') << std::endl;

    // Total
    std::cout << std::left << std::setw(20) << "Total Score"
        << std::right << std::setw(15) << std::fixed << std::setprecision(1) << score1.total_score
        << std::setw(15) << std::fixed << std::setprecision(1) << score2.total_score
        << std::setw(15) << (score1.total_score > score2.total_score ? name1 :
            score1.total_score < score2.total_score ? name2 : "Tie") << std::endl;

    // GPU
    std::cout << std::left << std::setw(20) << "GPU"
        << std::right << std::setw(15) << std::fixed << std::setprecision(1) << score1.gpu_score
        << std::setw(15) << std::fixed << std::setprecision(1) << score2.gpu_score
        << std::setw(15) << (score1.gpu_score > score2.gpu_score ? name1 :
            score1.gpu_score < score2.gpu_score ? name2 : "Tie") << std::endl;

    // VRAM
    std::cout << std::left << std::setw(20) << "VRAM"
        << std::right << std::setw(15) << std::fixed << std::setprecision(1) << score1.vram_score
        << std::setw(15) << std::fixed << std::setprecision(1) << score2.vram_score
        << std::setw(15) << (score1.vram_score > score2.vram_score ? name1 :
            score1.vram_score < score2.vram_score ? name2 : "Tie") << std::endl;

    // CPU
    std::cout << std::left << std::setw(20) << "CPU"
        << std::right << std::setw(15) << std::fixed << std::setprecision(1) << score1.cpu_score
        << std::setw(15) << std::fixed << std::setprecision(1) << score2.cpu_score
        << std::setw(15) << (score1.cpu_score > score2.cpu_score ? name1 :
            score1.cpu_score < score2.cpu_score ? name2 : "Tie") << std::endl;

    // RAM
    std::cout << std::left << std::setw(20) << "RAM"
        << std::right << std::setw(15) << std::fixed << std::setprecision(1) << score1.ram_score
        << std::setw(15) << std::fixed << std::setprecision(1) << score2.ram_score
        << std::setw(15) << (score1.ram_score > score2.ram_score ? name1 :
            score1.ram_score < score2.ram_score ? name2 : "Tie") << std::endl;

    // Disk
    std::cout << std::left << std::setw(20) << "Disk"
        << std::right << std::setw(15) << std::fixed << std::setprecision(1) << score1.disk_score
        << std::setw(15) << std::fixed << std::setprecision(1) << score2.disk_score
        << std::setw(15) << (score1.disk_score > score2.disk_score ? name1 :
            score1.disk_score < score2.disk_score ? name2 : "Tie") << std::endl;

    std::cout << std::string(65, '-') << std::endl;

    // Визначаємо переможця
    std::string overall_winner;
    double diff = std::abs(score1.total_score - score2.total_score);

    if (diff < 5.0) {
        overall_winner = "Systems are very similar";
    }
    else if (score1.total_score > score2.total_score) {
        overall_winner = name1 + " is better by " +
            std::to_string((int)diff) + " points";
    }
    else {
        overall_winner = name2 + " is better by " +
            std::to_string((int)diff) + " points";
    }

    std::cout << std::endl;
    std::cout << "Verdict: " << overall_winner << std::endl;
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
}