#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstdlib>

using namespace std;

class Tensor {
public:
    double* data;
    vector<size_t> shape;

    // 1. Constructor Vacío
    Tensor() : data(nullptr), shape({}) {}

    // 2. Constructor Principal
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

    // 3. Constructor de Copia
    Tensor(const Tensor& other) {
        this->shape = other.shape;
        if (other.data != nullptr) {
            size_t total = 1;
            for (size_t dim : shape) total *= dim;

            this->data = new double[total];
            for (size_t i = 0; i < total; ++i) {
                this->data[i] = other.data[i];
            }
        } else {
            this->data = nullptr;
        }
        cout << "Clase Tensor: Constructor Copia -> " << this << endl;
    }

    // 4. Constructor de Movimiento
    Tensor(Tensor&& other) noexcept {
        this->data = other.data;
        this->shape = move(other.shape);
        other.data = nullptr;
        other.shape.clear();

        cout << "Clase Tensor: Constructor Movimiento -> " << this << endl;
    }

    // 5. Asignador de Copia
    Tensor& operator=(const Tensor& other) {
        if (this != &other) {
            delete[] this->data;

            this->shape = other.shape;
            if (other.data != nullptr) {
                size_t total = 1;
                for (size_t dim : shape) total *= dim;
                this->data = new double[total];
                for (size_t i = 0; i < total; ++i) {
                    this->data[i] = other.data[i];
                }
            } else {
                this->data = nullptr;
            }
        }
        return *this;
    }

    // 6. Asignador de Movimiento
    Tensor& operator=(Tensor&& other) noexcept {
        if (this != &other) {
            delete[] this->data;

            this->data = other.data;
            this->shape = move(other.shape);

            other.data = nullptr;
            other.shape.clear();
        }
        return *this;
    }

    // 7. Destructor (Sección 4)
    ~Tensor() {
        delete[] data;
    }

    //  Métodos Estáticos
    static Tensor zeros(const vector<size_t>& shape) {
        size_t total_size = 1;
        for (size_t dim : shape) total_size *= dim;
        vector<double> ceros(total_size, 0.0);
        return Tensor(shape, ceros);
    }

    static Tensor ones(const vector<size_t>& shape) {
        size_t total_size = 1;
        for (size_t dim : shape) total_size *= dim;
        vector<double> ones_vec(total_size, 1.0);
        return Tensor(shape, ones_vec);
    }

    static Tensor random(const vector<size_t>& shape, double min = 0.0, double max = 1.0) {
        size_t total_size = 1;
        for (size_t dim : shape) total_size *= dim;

        vector<double> vals(total_size);
        for (size_t i = 0; i < total_size; ++i) {
            double r = (double)rand() / RAND_MAX;
            vals[i] = min + r * (max - min);
        }
        return Tensor(shape, vals);
    }

    static Tensor arange(double start, double end, double step = 1.0) {
        vector<double> vals;
        for (double v = start; v < end; v += step) {
            vals.push_back(v);
        }
        vector<size_t> shape = { vals.size() };
        return Tensor(shape, vals);
    }

    // Auxiliar de Indexación
    size_t get_index(size_t i, size_t j = 0, size_t k = 0) const {
        if (shape.size() == 1) return i;
        if (shape.size() == 2) return i * shape[1] + j;
        return i * (shape[1] * shape[2]) + j * shape[2] + k;
    }
};

int main() {
    Tensor A;
    Tensor B = Tensor::zeros({2, 3});

    A = B; // Asignación copia
    Tensor C = std::move(B); // Movimiento

    cout << "Valor en index (0,1) de C: " << C.data[C.get_index(0, 1)] << endl;
    return 0;
}