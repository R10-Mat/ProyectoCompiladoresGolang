import matplotlib.pyplot as plt
import numpy as np

# 1. Definir los datos del benchmark con nombres descriptivos
archivos = [
    'Fibonacci\n(Recursividad)', 
    'Bubble Sort\n(Arreglos/Bucles)', 
    'Mutación Structs\n(Punteros)', 
    'Switch Masivo\n(Saltos)', 
    'Criba Eratóstenes\n(Lógica/Arreglos)', 
    'Ackermann\n(Stack/Recursividad)', 
    'Métodos\n(Receptores PTR)', 
    'Collatz\n(Módulo/Div)', 
    'Matrices\n(Índices Complejos)', 
    'Structs\n(Lit. Compuestos)'
]

tiempo_tu_compilador = [
    0.17751, 0.05656, 0.08898, 0.04851, 0.04609, 
    0.03642, 0.04075, 0.04680, 0.03439, 0.04295
]

tiempo_go_oficial = [
    1.17667, 2.13329, 2.86465, 2.20314, 1.61064, 
    2.13237, 1.57821, 1.23606, 1.25557, 1.50298
]

# 2. Configurar las posiciones y el ancho de las barras
x = np.arange(len(archivos))
ancho_barra = 0.35

# 3. Crear la figura y los ejes (hacemos la figura un poco más ancha para los textos)
fig, ax = plt.subplots(figsize=(15, 8))

# 4. Dibujar las barras
barras_tuyas = ax.bar(x - ancho_barra/2, tiempo_tu_compilador, ancho_barra, 
                      label='Tu Compilador (MiniGo)', color='#2ca02c', edgecolor='black')
barras_go = ax.bar(x + ancho_barra/2, tiempo_go_oficial, ancho_barra, 
                   label='Go Oficial', color='#1f77b4', edgecolor='black')

# 5. Personalizar el gráfico
ax.set_ylabel('Tiempo de Compilación (segundos)', fontsize=12, fontweight='bold')
ax.set_xlabel('Prueba (Característica Evaluada)', fontsize=12, fontweight='bold', labelpad=15)
ax.set_title('Benchmark de Tiempo de Compilación: Tu Compilador vs Go Oficial', fontsize=16, fontweight='bold', pad=20)
ax.set_xticks(x)
# Rotamos menos el texto porque ahora usamos saltos de línea (\n)
ax.set_xticklabels(archivos, rotation=0, ha='center', fontsize=9)
ax.legend(fontsize=12)

ax.grid(axis='y', linestyle='--', alpha=0.7)

# 6. Función para añadir el número exacto
def autolabel(rects):
    for rect in rects:
        alto = rect.get_height()
        ax.annotate(f'{alto:.3f}s',
                    xy=(rect.get_x() + rect.get_width() / 2, alto),
                    xytext=(0, 5),
                    textcoords="offset points",
                    ha='center', va='bottom', fontsize=9)

# En este caso, como tus tiempos son muy bajos, podemos rotar los de Go si se superponen,
# pero con este tamaño de figura deberían caber bien en horizontal.
autolabel(barras_tuyas)
autolabel(barras_go)

fig.tight_layout()

# 7. Guardar y mostrar
plt.savefig('grafico_benchmark_descriptivo.png', dpi=300, bbox_inches='tight')
print("Gráfico guardado exitosamente como 'grafico_benchmark_descriptivo.png'")
plt.show()