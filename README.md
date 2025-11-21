# PBO-Simulator-Rental-Kendaraan
**Studi Kasus 2:** Sistem Rental Kendaraan dengan Performa & Fault Handling
**Mata Kuliah:** SIR201 – Pemrograman Berorientasi Objek (PBO)
**Program Studi:** Teknik Robotika & Kecerdasan Buatan – Universitas Airlangga

---
## Video Penjelasan Project
Video penjelasan terkait alur program, hierarki class, exception handling, dynamic_cast, polymorphism, dan RAII Logger:

--- 

## Repository Project 
https://github.com/althaafridha/PBO-Simulator-Rental-Kendaraan/ 

---


| Nama          | NIM                | Peran                     | Tugas                                                                                          |
| ------------- | ------------------ | ------------------------- | ---------------------------------------------------------------------------------------------- |
| Aulia Rizky Arief Wicaksono | 163241025 | Vehicle Classes & Cloning | Implementasi `Vehicle`, `Car`, `Truck`, `ElectricCar`, method rentCost, dan clone.             |
| Muhammad Iqbal Alfaqih | 163241051 | Exception Rules           | Membuat custom exception hierarchy, mapping error → exception, kondisi battery low & overload. |
| Khanza Audriec Alva Javier | 163241053 | CLI, Logger, dan Pengujian            | Membuat menu CLI, input user, RAII Logger, try/catch handling, tampilan pesan user-friendly, dan Membuat test cases.   |
| Althaaf Ridha M. Zaydaan Batubara | 163241054 | Rental Manager Logic dan Dokumentasi   | Implementasi `rent`, `return`, `chargeBattery`, dan `dynamic_cast`, Membuat test cases, dokumentasi, README, dan video demo.                                     |

**Catatan Asumsi Program**
 - Format penyimpanan belum menggunakan database (hanya runtime memory dan file log).
 - Pengisian baterai hanya berlaku untuk ElectricCar.
 - Sistem belum mendukung penyimpanan otomatis ke CSV (opsional sesuai PDF).
