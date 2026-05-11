# 🪐 Atmos Browser
Um navegador minimalista e focado em performance, construído sobre o CEF (Chromium Embedded Framework).

## ✨ Características
- **Interface Dark Mode:** Focada em reduzir o cansaço visual.
- **New Tab Page Customizada:** Minimalismo puro com foco em busca direta.
- **Engine Chromium:** Compatibilidade total com a web moderna.

## 🚀 Como Compilar
1. Clone o repositório.
2. Certifique-se de ter o CMake e o VS2022 instalados.
3. Execute:
   ```bash
   mkdir build && cd build
   cmake -G "Visual Studio 17 2022" -A x64 ..
   cmake --build . --config Release --target Atmos
   ```

## 🛠️ Status do Projeto
Atualmente em fase alpha. Ajustes de interface e ativação nativa do Chrome Runtime em progresso.
