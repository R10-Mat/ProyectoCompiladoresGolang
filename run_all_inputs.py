import glob
import os
import re
import subprocess
import shutil

# Archivos c++ (scanner -> parser -> typechecker -> generacion de codigo x86-64)
programa = [
    "main.cpp", "scanner.cpp", "token.cpp", "parser.cpp", "ast.cpp",
    "visitor.cpp", "Semantic_types.cpp", "TypeCheker.cpp", "GenCode.cpp",
]

# Compilar
compile = ["g++", "-std=c++20"] + programa
print("Compilando:", " ".join(compile))
result = subprocess.run(compile, capture_output=True, text=True)

if result.returncode != 0:
    print("Error en compilación:\n", result.stderr)
    exit(1)

print("Compilación exitosa")

# Ejecutar
input_dir = "inputs"
output_dir = "outputs"
os.makedirs(output_dir, exist_ok=True)

# Descubre todos los inputN.txt presentes en inputs/ (no un rango fijo:
# la carpeta puede tener muchos archivos).
patron = re.compile(r"input(\d+)\.txt$")
archivos = []
for filepath in glob.glob(os.path.join(input_dir, "input*.txt")):
    m = patron.search(os.path.basename(filepath))
    if m:
        archivos.append((int(m.group(1)), filepath))
archivos.sort()

if not archivos:
    print(f"No se encontraron archivos input*.txt en {input_dir}/")
    exit(0)

for i, filepath in archivos:
    filename = os.path.basename(filepath)
    print(f"Ejecutando {filename}")
    run_cmd = ["./a.out", filepath]
    result = subprocess.run(run_cmd, capture_output=True, text=True)

    # output_i.txt: log de las 4 fases (lexica/sintactica/semantica/codegen),
    # OK o el error puntual — sirve para debugear cualquier input.
    output_file = os.path.join(output_dir, f"output{i}.txt")
    with open(output_file, "w", encoding="utf-8") as f:
        f.write("=== STDOUT ===\n")
        f.write(result.stdout)
        f.write("\n=== STDERR ===\n")
        f.write(result.stderr)

    # tokens_i.txt: volcado de la fase lexica (lo escribe ejecutar_scanner
    # en inputs/, se mueve a outputs/ con el indice correcto).
    tokens_file = os.path.join(input_dir, f"input{i}_tokens.txt")
    if os.path.isfile(tokens_file):
        shutil.move(tokens_file, os.path.join(output_dir, f"tokens_{i}.txt"))

    # output_i.s: ensamblador x86-64 generado (main.cpp siempre lo escribe
    # como "output.s" en la raiz; solo existe si se llego a la fase de
    # codegen, es decir si las 3 fases anteriores fueron OK).
    asm_file = "output.s"
    if os.path.isfile(asm_file):
        shutil.move(asm_file, os.path.join(output_dir, f"output_{i}.s"))

    # AST (si algun dia se agrega volcado a .dot)
    ast_file = "ast.dot"
    if os.path.isfile(ast_file):
        dest_ast = os.path.join(output_dir, f"ast_{i}.dot")
        shutil.move(ast_file, dest_ast)
        output_img = os.path.join(output_dir, f"ast_{i}.png")
        dot_cmd = ["dot", "-Tpng", dest_ast, "-o", output_img]
        subprocess.run(dot_cmd, capture_output=True, text=True)
