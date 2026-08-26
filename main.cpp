#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

class Tensor {
public:
    double* data;
    vector<size_t> shape;
    bool owns_data;

    //Constructor por Defecto
    Tensor() : data(nullptr), shape({}) {}

    // 1. Constructor Principal
    Tensor(const vector<size_t>& shape, const vector<double>& values) {
        if (shape.empty() || shape.size() > 3) {
            throw invalid_argument("El tensor debe tener entre 1 y 3 dimensiones.");
        }

        this->shape = shape;
        owns_data = true;
        size_t total_size = 1;
        for (size_t dim : shape) {
            if (dim == 0) {
                throw invalid_argument("Las dimensiones deben ser mayores que cero.");
            }
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

    // Constructor para vistas: reutiliza la memoria existente sin copiarla.
    Tensor(const vector<size_t>& shape, double* data, bool owns_data) {
        if (shape.empty() || shape.size() > 3) {
            throw invalid_argument("El tensor debe tener entre 1 y 3 dimensiones.");
        }

        this->shape = shape;
        this->data = data;
        this->owns_data = owns_data;
    }

    // Constructor de copia: hace deep copy para evitar compartir memoria accidentalmente.
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

    // Constructor de movimiento: transfiere la propiedad del arreglo.
    Tensor(Tensor&& other) noexcept {
        this->data = other.data;
        this->shape = move(other.shape);
        other.data = nullptr;
        other.shape.clear();

        cout << "Clase Tensor: Constructor Movimiento -> " << this << endl;
    }

    // Asignador de copia.
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

    // Asignador de movimiento.
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

    // Destructor (DENTRO de la clase, Seccion 4)
    ~Tensor() {
        delete[] data;
    }

    size_t size() const {
        size_t total_size = 1;
        for (size_t dim : shape) {
            total_size *= dim;
        }
        return total_size;
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
        if (min > max) {
            throw invalid_argument("El minimo no puede ser mayor que el maximo.");
        }
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
        for (double v = start; v < end; v += step) {
            vals.push_back(v);
        }
        vector<size_t> shape = { vals.size() };
        return Tensor(shape, vals);
    }

    Tensor operator+(const Tensor& other) const {
        vector<double> result(size());

        if (shape == other.shape) {
            for (size_t i = 0; i < size(); ++i) {
                result[i] = data[i] + other.data[i];
            }
            return Tensor(shape, result);
        }

        if (shape.size() == 2 && other.shape.size() == 2 && other.shape[0] == 1 && shape[1] == other.shape[1]) {
            for (size_t i = 0; i < shape[0]; ++i) {
                for (size_t j = 0; j < shape[1]; ++j) {
                    result[i * shape[1] + j] = data[i * shape[1] + j] + other.data[j];
                }
            }
            return Tensor(shape, result);
        }

        throw invalid_argument("Los tensores deben tener dimensiones compatibles.");
    }

    Tensor operator-(const Tensor& other) const {
        validate_same_shape(other);

        vector<double> result(size());
        for (size_t i = 0; i < size(); ++i) {
            result[i] = data[i] - other.data[i];
        }
        return Tensor(shape, result);
    }

    Tensor operator*(const Tensor& other) const {
        validate_same_shape(other);

        vector<double> result(size());
        for (size_t i = 0; i < size(); ++i) {
            result[i] = data[i] * other.data[i];
        }
        return Tensor(shape, result);
    }

    Tensor operator/(const Tensor& other) const {
        validate_same_shape(other);

        vector<double> result(size());
        for (size_t i = 0; i < size(); ++i) {
            if (other.data[i] == 0.0) {
                throw invalid_argument("No se puede dividir entre cero.");
            }
            result[i] = data[i] / other.data[i];
        }
        return Tensor(shape, result);
    }

    Tensor view(const vector<size_t>& new_shape) const {
        size_t new_total = 1;
        for (size_t dim : new_shape) {
            new_total *= dim;
        }

        if (new_shape.empty() || new_shape.size() > 3 || new_total != size()) {
            throw invalid_argument("El nuevo shape no es compatible.");
        }

        return Tensor(new_shape, data, false);
    }

    Tensor unsqueeze(size_t dim) const {
        if (shape.size() == 3 || dim > shape.size()) {
            throw invalid_argument("No se puede agregar esa dimension.");
        }

        vector<size_t> new_shape = shape;
        new_shape.insert(new_shape.begin() + dim, 1);
        return Tensor(new_shape, data, false);
    }

    static Tensor concat(const vector<Tensor>& tensors, size_t dim) {
        if (tensors.empty()) {
            throw invalid_argument("Se necesita al menos un tensor para concatenar.");
        }
        if (dim >= tensors[0].shape.size()) {
            throw invalid_argument("Dimension de concatenacion invalida.");
        }

        vector<size_t> result_shape = tensors[0].shape;
        result_shape[dim] = 0;

        for (const Tensor& tensor : tensors) {
            if (tensor.shape.size() != tensors[0].shape.size()) {
                throw invalid_argument("Todos los tensores deben tener la misma cantidad de dimensiones.");
            }
            for (size_t i = 0; i < tensor.shape.size(); ++i) {
                if (i != dim && tensor.shape[i] != tensors[0].shape[i]) {
                    throw invalid_argument("Los shapes no son compatibles para concatenar.");
                }
            }
            result_shape[dim] += tensor.shape[dim];
        }

        vector<double> result;
        size_t result_total = 1;
        for (size_t size_dim : result_shape) {
            result_total *= size_dim;
        }
        result.resize(result_total);

        size_t offset = 0;
        for (const Tensor& tensor : tensors) {
            for (size_t i = 0; i < tensor.shape[0]; ++i) {
                for (size_t j = 0; j < (tensor.shape.size() >= 2 ? tensor.shape[1] : 1); ++j) {
                    for (size_t k = 0; k < (tensor.shape.size() == 3 ? tensor.shape[2] : 1); ++k) {
                        size_t dest_i = i + (dim == 0 ? offset : 0);
                        size_t dest_j = j + (dim == 1 ? offset : 0);
                        size_t dest_k = k + (dim == 2 ? offset : 0);
                        result[linear_index(result_shape, dest_i, dest_j, dest_k)] = tensor.data[tensor.get_index(i, j, k)];
                    }
                }
            }
            offset += tensor.shape[dim];
        }

        return Tensor(result_shape, result);
    }

    // Auxiliar de Indexación
    size_t get_index(size_t i, size_t j = 0, size_t k = 0) const {
        if (shape.size() == 1) return i;
        if (shape.size() == 2) return i * shape[1] + j;
        return i * (shape[1] * shape[2]) + j * shape[2] + k;
    }

    void print_size(const string& name) const {
        cout << name << " size: ";
        for (size_t i = 0; i < shape.size(); ++i) {
            cout << shape[i];
            if (i + 1 < shape.size()) {
                cout << " x ";
            }
        }
        cout << endl;
    }

    friend Tensor dot(const Tensor& a, const Tensor& b);
    friend Tensor matmul(const Tensor& a, const Tensor& b);

private:
    void validate_same_shape(const Tensor& other) const {
        if (shape != other.shape) {
            throw invalid_argument("Los tensores deben tener dimensiones compatibles.");
        }
    }

    static size_t linear_index(const vector<size_t>& tensor_shape, size_t i, size_t j = 0, size_t k = 0) {
        if (tensor_shape.size() == 1) return i;
        if (tensor_shape.size() == 2) return i * tensor_shape[1] + j;
        return i * (tensor_shape[1] * tensor_shape[2]) + j * tensor_shape[2] + k;
    }
};

Tensor dot(const Tensor& a, const Tensor& b) {
    if (a.shape != b.shape) {
        throw invalid_argument("dot requiere tensores con la misma dimension.");
    }

    double sum = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        sum += a.data[i] * b.data[i];
    }

    return Tensor({1}, {sum});
}

Tensor matmul(const Tensor& a, const Tensor& b) {
    if (a.shape.size() != 2 || b.shape.size() != 2) {
        throw invalid_argument("matmul requiere tensores bidimensionales.");
    }
    if (a.shape[1] != b.shape[0]) {
        throw invalid_argument("Las dimensiones no son compatibles para matmul.");
    }

    vector<size_t> result_shape = {a.shape[0], b.shape[1]};
    vector<double> result(a.shape[0] * b.shape[1], 0.0);

    for (size_t i = 0; i < a.shape[0]; ++i) {
        for (size_t j = 0; j < b.shape[1]; ++j) {
            for (size_t k = 0; k < a.shape[1]; ++k) {
                result[i * b.shape[1] + j] += a.data[i * a.shape[1] + k] * b.data[k * b.shape[1] + j];
            }
        }
    }

    return Tensor(result_shape, result);
}

Tensor relu(const Tensor& tensor) {
    vector<double> result(tensor.size());
    for (size_t i = 0; i < tensor.size(); ++i) {
        result[i] = tensor.data[i] > 0.0 ? tensor.data[i] : 0.0;
    }
    return Tensor(tensor.shape, result);
}

Tensor sigmoid(const Tensor& tensor) {
    vector<double> result(tensor.size());
    for (size_t i = 0; i < tensor.size(); ++i) {
        result[i] = 1.0 / (1.0 + exp(-tensor.data[i]));
    }
    return Tensor(tensor.shape, result);
}

int main() {
    Tensor Z;
    Tensor Y = Tensor::zeros({2, 3});

    Z = Y; // Asignación copia
    Tensor X = std::move(Y); // Movimiento

    cout << "Valor en index (0,1) de X: " << X.data[X.get_index(0, 1)] << endl;

    // Uso correcto del método estático para fabricar un Tensor de 2x3 lleno de 0.0:

    Tensor A = Tensor::ones({2, 3});
    Tensor B = Tensor::zeros({2, 3});
    Tensor C = A + B;
    Tensor D = A - B;
    Tensor E = A * C;
    Tensor F = C / A;

    Tensor G = Tensor::arange(0, 12);
    Tensor H = G.view({3, 4});
    Tensor I = G.unsqueeze(0);
    Tensor J = Tensor::concat({A, B}, 0);

    Tensor K = dot(Tensor::ones({3}), Tensor::arange(1, 4));
    Tensor L = matmul(Tensor::ones({2, 3}), Tensor::ones({3, 2}));

    cout << "Operadores y transformaciones:" << endl;
    C.print_size("A + B");
    D.print_size("A - B");
    E.print_size("A * C");
    F.print_size("C / A");
    H.print_size("view");
    I.print_size("unsqueeze");
    J.print_size("concat");
    K.print_size("dot");
    L.print_size("matmul");

    cout << "\nRed neuronal:" << endl;
    Tensor entrada = Tensor::random({1000, 20, 20});
    entrada.print_size("Entrada");

    Tensor capa0 = entrada.view({1000, 400});
    capa0.print_size("View");

    Tensor w1 = Tensor::random({400, 100});
    Tensor capa1 = matmul(capa0, w1);
    capa1.print_size("Matmul 400x100");

    Tensor b1 = Tensor::ones({1, 100});
    Tensor z1 = capa1 + b1;
    z1.print_size("Bias 1x100");

    Tensor a1 = relu(z1);
    a1.print_size("ReLU");

    Tensor w2 = Tensor::random({100, 10});
    Tensor capa2 = matmul(a1, w2);
    capa2.print_size("Matmul 100x10");

    Tensor b2 = Tensor::ones({1, 10});
    Tensor z2 = capa2 + b2;
    z2.print_size("Bias 1x10");

    Tensor salida = sigmoid(z2);
    salida.print_size("Sigmoid");
    return 0;
};
