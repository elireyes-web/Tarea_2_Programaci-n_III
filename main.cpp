#include <iostream>
#include <vector>
#include <stdexcept>
using namespace std;

class Tensor {
public:
    double* data;
    vector<size_t> shape;

    // 1. Constructor Principal
    Tensor(const vector<size_t>& shape, const vector<double>& values) {
        this->shape = shape;
        size_t total_size = 1;
        for (size_t dim : shape) {
            total_size *= dim;
        }

        if (values.size() != total_size) {
            throw invalid_argument("El numero de valores no coincide con el shape.");
        }

        data = new double[total_size];
        for (size_t i = 0; i < total_size; ++i) {
            data[i] = values[i];
        }
    }

    // 2. Destructor (DENTRO de la clase)
    ~Tensor() {
        delete[] data;
    }

    // 3. Método ESTÁTICO (Recibe el shape, ej: {2, 3})
    static Tensor zeros(const vector<size_t>& shape) {
        size_t total_size = 1;
        for (size_t dim : shape) {
            total_size *= dim;
        }

        // Crea el vector relleno de ceros
        vector<double> ceros(total_size, 0.0);

        // Llama al constructor principal usando (shape, ceros)
        return Tensor(shape, ceros);
    }

    static Tensor ones(const vector<size_t>& shape) {
        size_t total_size = 1;
        for (size_t dim : shape) {
            total_size *= dim;
        }

        // Crea el vector relleno de ceros
        vector<double> ones(total_size, 1.0);

        // Llama al constructor principal usando (shape, ceros)
        return Tensor(shape, ones);
    }

    static Tensor random(const vector<size_t>& shape, double min = 0.0, double max = 1.0) {
        size_t total_size = 1;
        for (size_t dim : shape) {
            total_size *= dim;
        }

        vector<double> vals(total_size);
        for (size_t i = 0; i < total_size; ++i) {
            // Genera un número decimal uniforme entre min y max
            double r = (double)rand() / RAND_MAX;
            vals[i] = min + r * (max - min);
        }

        return Tensor(shape, vals);
    }

    static Tensor arange(double start, double end, double step = 1.0) {
        vector<double> vals;

        // Genera la secuencia desde 'start' hasta antes de 'end'
        for (double v = start; v < end; v += step) {
            vals.push_back(v);
        }

        // La dimensión de un tensor 1D es { cantidad_de_elementos }
        vector<size_t> shape = { vals.size() }; 

        return Tensor(shape, vals);
    }

    size_t get_index(size_t i, size_t j = 0, size_t k = 0) const {
        if (shape.size() == 1) return i;
        if (shape.size() == 2) return i * shape[1] + j;
        return i * (shape[1] * shape[2]) + j * shape[2] + k;
    }
};

int main() {
    // Uso correcto del método estático para fabricar un Tensor de 2x3 lleno de 0.0:
    Tensor T = Tensor::zeros({2, 3});

    cout << "Index (0,1): " << T.get_index(0, 1) << endl;
    return 0;
};