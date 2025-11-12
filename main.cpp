#include <QCoreApplication>
#include <QDebug>
#include <iostream>
#include <iomanip>
#include "HardwareInfoProvider.h"
#include "SystemScorer.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    std::cout << "\n";
    std::cout << "=====================================" << std::endl;
    std::cout << "  Hardware Info Provider v5.0" << std::endl;
    std::cout << "  With SystemScorer" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "\n";

    // ========================================
    // Створюємо провайдер
    // ========================================
    HardwareInfoProvider hw;

    // ========================================
    // Отримуємо структуру ArgentumDevice
    // ========================================
    std::cout << "Collecting device information..." << std::endl;
    ArgentumDevice device = hw.getDeviceInfo();
    std::cout << "Done!" << std::endl;
    std::cout << "\n";

    // ========================================
    // Виводимо базову інформацію
    // ========================================
    HardwareInfoProvider::printDeviceInfo(device);

    // ========================================
    // СИСТЕМА ОЦІНЮВАННЯ
    // ========================================

    std::cout << "\n";
    std::cout << "=====================================" << std::endl;
    std::cout << "  SYSTEM SCORING" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "\n";

    SystemScorer scorer;

    // ========================================
    // 1. Оцінка для майнінгу
    // ========================================
    std::cout << "\n>>> MINING PERFORMANCE <<<" << std::endl;
    SystemScore mining_score = scorer.scoreSystem(
        device,
        SystemScorer::getMiningWeights(),
        "mining"
    );
    SystemScorer::printDetailedScore(mining_score);

    // ========================================
    // 2. Оцінка для AI Training
    // ========================================
    std::cout << "\n>>> AI TRAINING PERFORMANCE <<<" << std::endl;
    SystemScore ai_score = scorer.scoreSystem(
        device,
        SystemScorer::getAITrainingWeights(),
        "ai_training"
    );
    SystemScorer::printScore(ai_score);

    // ========================================
    // 3. Оцінка для Video Rendering
    // ========================================
    std::cout << "\n>>> VIDEO RENDERING PERFORMANCE <<<" << std::endl;
    SystemScore video_score = scorer.scoreSystem(
        device,
        SystemScorer::getVideoRenderingWeights(),
        "video_rendering"
    );
    SystemScorer::printScore(video_score);

    // ========================================
    // 4. Оцінка для Gaming
    // ========================================
    std::cout << "\n>>> GAMING PERFORMANCE <<<" << std::endl;
    SystemScore gaming_score = scorer.scoreSystem(
        device,
        SystemScorer::getGamingWeights(),
        "gaming"
    );
    SystemScorer::printScore(gaming_score);

    // ========================================
    // 5. Загальна оцінка
    // ========================================
    std::cout << "\n>>> GENERAL PURPOSE PERFORMANCE <<<" << std::endl;
    SystemScore general_score = scorer.scoreSystem(
        device,
        SystemScorer::getGeneralWeights(),
        "general"
    );
    SystemScorer::printScore(general_score);

    // ========================================
    // ПОРІВНЯННЯ ЗАВДАНЬ
    // ========================================
    std::cout << "\n";
    std::cout << "=====================================" << std::endl;
    std::cout << "  TASK COMPARISON" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "\n";

    std::cout << std::left << std::setw(25) << "Task"
        << std::right << std::setw(15) << "Score"
        << std::setw(15) << "Rating" << std::endl;
    std::cout << std::string(55, '-') << std::endl;

    std::cout << std::left << std::setw(25) << "Mining"
        << std::right << std::setw(15) << std::fixed << std::setprecision(1) << mining_score.total_score
        << std::setw(15) << mining_score.rating << std::endl;

    std::cout << std::left << std::setw(25) << "AI Training"
        << std::right << std::setw(15) << std::fixed << std::setprecision(1) << ai_score.total_score
        << std::setw(15) << ai_score.rating << std::endl;

    std::cout << std::left << std::setw(25) << "Video Rendering"
        << std::right << std::setw(15) << std::fixed << std::setprecision(1) << video_score.total_score
        << std::setw(15) << video_score.rating << std::endl;

    std::cout << std::left << std::setw(25) << "Gaming"
        << std::right << std::setw(15) << std::fixed << std::setprecision(1) << gaming_score.total_score
        << std::setw(15) << gaming_score.rating << std::endl;

    std::cout << std::left << std::setw(25) << "General Purpose"
        << std::right << std::setw(15) << std::fixed << std::setprecision(1) << general_score.total_score
        << std::setw(15) << general_score.rating << std::endl;

    std::cout << std::string(55, '-') << std::endl;

    // Знаходимо найкращий варіант
    struct TaskResult {
        std::string name;
        double score;
    };

    std::vector<TaskResult> results = {
        {"Mining", mining_score.total_score},
        {"AI Training", ai_score.total_score},
        {"Video Rendering", video_score.total_score},
        {"Gaming", gaming_score.total_score},
        {"General Purpose", general_score.total_score}
    };

    auto best = std::max_element(results.begin(), results.end(),
        [](const TaskResult& a, const TaskResult& b) {
            return a.score < b.score;
        });

    std::cout << std::endl;
    std::cout << "Best Use Case: " << best->name
        << " (" << std::fixed << std::setprecision(1) << best->score << "/100)" << std::endl;
    std::cout << std::endl;

    // ========================================
    // ПРИКЛАД ПОРІВНЯННЯ ДВОХ СИСТЕМ
    // ========================================
    std::cout << "\n";
    std::cout << "=====================================" << std::endl;
    std::cout << "  EXAMPLE: COMPARING TWO SYSTEMS" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "\n";

    // Створюємо віртуальну систему 2 для порівняння
    ArgentumDevice device2;
    device2.os = "Windows 11 Pro";
    device2.cpu_model = "Intel Core i9-13900K";
    device2.cpu_cores = 24;
    device2.cpu_frequency_mhz = 3000;
    device2.ram_mb = 65536; // 64 GB

    GPUInfo gpu2;
    gpu2.model = "NVIDIA RTX 4080";
    gpu2.vram_mb = 16384; // 16 GB
    device2.gpus.push_back(gpu2);
    device2.gpu_count = 1;

    device2.primary_disk_type = DiskType::SSD;
    device2.free_disk_mb = 512000; // 500 GB

    // Оцінюємо обидві системи для майнінгу
    SystemScore system1_mining = scorer.scoreSystem(device, SystemScorer::getMiningWeights(), "mining");
    SystemScore system2_mining = scorer.scoreSystem(device2, SystemScorer::getMiningWeights(), "mining");

    SystemScorer::printComparison(system1_mining, system2_mining, "Your System", "RTX 4080 System");

    // ========================================
    // GPU DATABASE INFO
    // ========================================
    std::cout << "\n";
    std::cout << "=====================================" << std::endl;
    std::cout << "  GPU DATABASE LOOKUP" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "\n";

    if (!device.gpus.empty()) {
        auto gpu_info = scorer.findGPU(device.gpus[0].model);
        if (gpu_info.has_value()) {
            const GPUBenchmark& gpu = gpu_info.value();
            std::cout << "Your GPU: " << device.gpus[0].model << std::endl;
            std::cout << "Database Match: " << gpu.model_pattern << std::endl;
            std::cout << "Estimated Hashrate: " << std::fixed << std::setprecision(1)
                << gpu.hashrate << " MH/s (Ethereum)" << std::endl;
            std::cout << "Power Efficiency: " << std::fixed << std::setprecision(2)
                << gpu.power_efficiency << " MH/W" << std::endl;
            std::cout << "Performance Tier: " << gpu.tier << "/5 (1 = best)" << std::endl;
        }
        else {
            std::cout << "Your GPU (" << device.gpus[0].model
                << ") not found in database." << std::endl;
            std::cout << "Using default scoring." << std::endl;
        }
    }

    std::cout << std::endl;

    // ========================================
    // WEIGHTED SCORE BREAKDOWN (для розуміння)
    // ========================================
    std::cout << "\n";
    std::cout << "=====================================" << std::endl;
    std::cout << "  MINING SCORE CALCULATION BREAKDOWN" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "\n";

    TaskWeights mining_weights = SystemScorer::getMiningWeights();

    std::cout << "Component Scores × Weights = Contribution" << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    double gpu_contribution = mining_score.gpu_score * mining_weights.gpu_weight;
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "GPU:  " << std::setw(5) << mining_score.gpu_score << " × "
        << std::setprecision(2) << mining_weights.gpu_weight
        << " = " << std::setprecision(1) << std::setw(5) << gpu_contribution << std::endl;

    double vram_contribution = mining_score.vram_score * mining_weights.vram_weight;
    std::cout << "VRAM: " << std::setw(5) << mining_score.vram_score << " × "
        << std::setprecision(2) << mining_weights.vram_weight
        << " = " << std::setprecision(1) << std::setw(5) << vram_contribution << std::endl;

    double cpu_contribution = mining_score.cpu_score * mining_weights.cpu_weight;
    std::cout << "CPU:  " << std::setw(5) << mining_score.cpu_score << " × "
        << std::setprecision(2) << mining_weights.cpu_weight
        << " = " << std::setprecision(1) << std::setw(5) << cpu_contribution << std::endl;

    double ram_contribution = mining_score.ram_score * mining_weights.ram_weight;
    std::cout << "RAM:  " << std::setw(5) << mining_score.ram_score << " × "
        << std::setprecision(2) << mining_weights.ram_weight
        << " = " << std::setprecision(1) << std::setw(5) << ram_contribution << std::endl;

    double disk_contribution = mining_score.disk_score * mining_weights.disk_weight;
    std::cout << "Disk: " << std::setw(5) << mining_score.disk_score << " × "
        << std::setprecision(2) << mining_weights.disk_weight
        << " = " << std::setprecision(1) << std::setw(5) << disk_contribution << std::endl;

    std::cout << std::string(50, '-') << std::endl;
    std::cout << "Total Score: " << std::setprecision(1) << mining_score.total_score << "/100" << std::endl;
    std::cout << std::endl;

    // ========================================
    // ЗАВЕРШЕННЯ
    // ========================================
    std::cout << "\n";
    std::cout << "=====================================" << std::endl;
    std::cout << "  Program finished successfully!" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "\n";

    return 0;
}