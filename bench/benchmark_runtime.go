package main

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"regexp"
	"sort"
	"strconv"
	"strings"
	"time"
)

type TestFile struct {
	Index int
	Path  string
	Name  string
}

func main() {
	inputDir := "../inputs"

	files, err := os.ReadDir(inputDir)
	if err != nil {
		fmt.Println("Error leyendo el directorio inputs:", err)
		return
	}

	re := regexp.MustCompile(`^input(\d+)\.txt$`)
	var testFiles []TestFile

	for _, file := range files {
		matches := re.FindStringSubmatch(file.Name())
		if len(matches) == 2 {
			index, _ := strconv.Atoi(matches[1])
			testFiles = append(testFiles, TestFile{
				Index: index,
				Path:  filepath.Join(inputDir, file.Name()),
				Name:  file.Name(),
			})
		}
	}

	sort.Slice(testFiles, func(i, j int) bool {
		return testFiles[i].Index < testFiles[j].Index
	})

	fmt.Printf("%-15s | %-20s | %-20s\n", "Archivo", "Tu Ejecutable (s)", "Ejecutable Go (s)")
	fmt.Println("----------------------------------------------------------------")

	for _, tf := range testFiles {
		tiempoTuyoStr := "Error/Crash"
		
		tuCmd := exec.Command("../a.exe", tf.Path)
		if err := tuCmd.Run(); err == nil {
			
			// Parche para GCC en Windows (Quita la linea de GNU-stack)
			if sBytes, errReadS := os.ReadFile("output.s"); errReadS == nil {
				sText := string(sBytes)
				sText = strings.ReplaceAll(sText, ".section .note.GNU-stack,\"\",@progbits", "")
				os.WriteFile("output.s", []byte(sText), 0644)
			}
			
			tuBinario := fmt.Sprintf("tu_binario_%d.exe", tf.Index)
			gccCmd := exec.Command("gcc", "output.s", "-o", tuBinario)
			
			if errGcc := gccCmd.Run(); errGcc == nil {
				absPathTuyo, _ := filepath.Abs(tuBinario)
				runCmd := exec.Command(absPathTuyo)
				
				inicioTuyo := time.Now()
				errRun := runCmd.Run()
				tiempoTuyo := time.Since(inicioTuyo)

				if errRun == nil {
					tiempoTuyoStr = fmt.Sprintf("%.5f", tiempoTuyo.Seconds())
				} else {
					tiempoTuyoStr = fmt.Sprintf("Crash: %v", errRun)
				}
				
				os.Remove(tuBinario)
			} else {
				tiempoTuyoStr = "Error GCC"
			}
		}

		os.Remove("output.s")

		content, errRead := os.ReadFile(tf.Path)
		tiempoGoStr := "Error Lectura"

		if errRead == nil {
			tempName := "temp_" + tf.Name + ".go"
			codigoGoOficial := "package main\n" + string(content)
			os.WriteFile(tempName, []byte(codigoGoOficial), 0644)

			goBinario := fmt.Sprintf("go_binario_%d.exe", tf.Index)
			goCmd := exec.Command("go", "build", "-o", goBinario, tempName)
			
			if errGo := goCmd.Run(); errGo == nil {
				absPathGo, _ := filepath.Abs(goBinario)
				runGoCmd := exec.Command(absPathGo)
				
				inicioGo := time.Now()
				errRunGo := runGoCmd.Run()
				tiempoGo := time.Since(inicioGo)

				if errRunGo == nil {
					tiempoGoStr = fmt.Sprintf("%.5f", tiempoGo.Seconds())
				} else {
					tiempoGoStr = fmt.Sprintf("Crash: %v", errRunGo) 
				}

				os.Remove(goBinario)
			} else {
				tiempoGoStr = "Error Go Build"
			}

			os.Remove(tempName)
		}

		fmt.Printf("%-15s | %-20s | %-20s\n", tf.Name, tiempoTuyoStr, tiempoGoStr)
	}
}