package main

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"regexp"
	"sort"
	"strconv"
	"time"
)

type TestFile struct {
	Index int
	Path  string
	Name  string
}

func main() {
	inputDir := "../inputs"
	outputDir := "../outputs"

	os.MkdirAll(outputDir, os.ModePerm)

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

	if len(testFiles) == 0 {
		fmt.Printf("No se encontraron archivos input*.txt en %s/\n", inputDir)
		return
	}

	sort.Slice(testFiles, func(i, j int) bool {
		return testFiles[i].Index < testFiles[j].Index
	})

	fmt.Printf("%-15s | %-20s | %-20s\n", "Archivo", "Tu Compilador (s)", "Go Oficial (s)")
	fmt.Println("----------------------------------------------------------------")

	for _, tf := range testFiles {
	
		tuCmd := exec.Command("../a.exe", tf.Path)

		inicioTuyo := time.Now()
		outputBytes, errTuyo := tuCmd.CombinedOutput()
		tiempoTuyo := time.Since(inicioTuyo)

		tiempoTuyoStr := fmt.Sprintf("%.5f", tiempoTuyo.Seconds())
		if errTuyo != nil {
			tiempoTuyoStr = "Error/Crash"
		}

		outTxtPath := filepath.Join(outputDir, fmt.Sprintf("output_%d.txt", tf.Index))
		os.WriteFile(outTxtPath, outputBytes, 0644)

		tokensFile := filepath.Join(inputDir, fmt.Sprintf("input%d_tokens.txt", tf.Index))
		if _, err := os.Stat(tokensFile); err == nil {
			os.Rename(tokensFile, filepath.Join(outputDir, fmt.Sprintf("tokens_%d.txt", tf.Index)))
		}

		if _, err := os.Stat("output.s"); err == nil {
			os.Rename("output.s", filepath.Join(outputDir, fmt.Sprintf("output_%d.s", tf.Index)))
		}

		if _, err := os.Stat("ast.dot"); err == nil {
			destDot := filepath.Join(outputDir, fmt.Sprintf("ast_%d.dot", tf.Index))
			os.Rename("ast.dot", destDot)

			destPng := filepath.Join(outputDir, fmt.Sprintf("ast_%d.png", tf.Index))
			exec.Command("dot", "-Tpng", destDot, "-o", destPng).Run()
		}

		content, errRead := os.ReadFile(tf.Path)
		tiempoGoStr := "Error Lectura"

		if errRead == nil {
			tempName := "temp_" + tf.Name + ".go"
			codigoGoOficial := "package main\n" + string(content)

			os.WriteFile(tempName, []byte(codigoGoOficial), 0644)

			goCmd := exec.Command("go", "build", "-o", "temp_bin.exe", tempName)

			inicioGo := time.Now()
			errGo := goCmd.Run()
			tiempoGo := time.Since(inicioGo)

			if errGo != nil {
				tiempoGoStr = "Error Go Build"
			} else {
				tiempoGoStr = fmt.Sprintf("%.5f", tiempoGo.Seconds())
			}

			os.Remove(tempName)
			os.Remove("temp_bin.exe")
		}

		fmt.Printf("%-15s | %-20s | %-20s\n", tf.Name, tiempoTuyoStr, tiempoGoStr)
	}
}
