#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <numeric>
#include <future>
using namespace std;
using namespace std::chrono_literals;

mutex mtx;

void ejemplo1() {
    int x = 0;
    int n = 100;
    for (int i = 0; i < n; ++i)
        x++;
    cout << x << endl;
}

void incrementar(int& x) {
//    mtx.lock();
    lock_guard lg(mtx);
    x = x + 1;
    // Otras Instrucciones
//    mtx.unlock();
}

void incrementar_2(int& x) {
//    mtx.lock();
    unique_lock ul(mtx, std::defer_lock);
    // Instrucciones iniciales
    ul.lock();
    x = x + 1;
//    ul.unlock();
    // Otras Instrucciones
//    mtx.unlock();
}

void ejemplo2() {
    int n_repeticiones = 150;
    int n_hilos = 200;
    vector<thread> vhilos(n_hilos);

    for (int i = 0; i < n_repeticiones; ++i) {
        int x = 0;
        for (auto& h: vhilos)   // Asignar las funciones a los hilos
            h = thread(incrementar, ref(x));
        for (auto& h: vhilos)   // Ejecucion de los joins
            h.join();
        if (x != n_hilos)
            cout << x << " ";
    }
}

void ejemplo3() {
    int n_repeticiones = 150;
    int n_hilos = 200;
    vector<thread> vhilos(n_hilos);

    for (int i = 0; i < n_repeticiones; ++i) {
        atomic<int> x = 0;
        for (auto& h: vhilos)   // Asignar las funciones a los hilos
            h = thread([&]{++x;});
        for (auto& h: vhilos)   // Ejecucion de los joins
            h.join();
        if (x != n_hilos)
            cout << x << " ";
    }
}

// Programación Paralela

template <typename Iterator, typename T= typename Iterator::value_type>
T sumar(Iterator start, Iterator stop) {
    T total = 0;
    while(start != stop)
        total += *start++;
    return total;
}

void ejemplo4() {
    vector<int> v(100);
    iota(begin(v), end(v), 1); // Autocompleta el contenedor a partir de 1 consecutivas 1, 2, 3, 4
    cout << sumar(begin(v), end(v)) << endl;
}

template <typename Iterator, typename T= typename Iterator::value_type>
void sumar_ref(Iterator start, Iterator stop, T& res) {
    res = 0;
    while(start != stop)
        res += *start++;
}

void ejemplo5() {
    // Datos de entrada
    vector<int> v(103);
    iota(begin(v), end(v), 1);

    // Referencias o variables de los hilos
    int n_hilos = 10;
    int n_valores = size(v);
    int rango = ceil(n_valores / static_cast<double>(n_hilos));  // ceil(1.1) => 2 ceil(3.5) => 4 // round (1.1) => 1, round(3.5) => 4

    // Vectores
    vector<thread> v_hilos(n_hilos);   // Hilos
    vector<int> v_res_hilos(n_hilos);  // Casillero donde cada hilo va almacenar su resultado (sub-total)

    auto it = begin(v);
    // Asignando expresión lambda a los hilos
    for (int i = 0; i < n_hilos; ++i) {
        if (distance(it, end(v)) < rango) rango = distance(it, end(v));
        v_hilos[i] = thread(sumar_ref<vector<int>::iterator>, it, next(it, rango), ref(v_res_hilos[i]));
        it = next(it, rango);
    }
    // Ejecuta el método join a cada hilo
    for (int i = 0; i < n_hilos; ++i) {
        v_hilos[i].join();
    }
    // Suma total
    auto total = sumar(begin(v_res_hilos), end(v_res_hilos));
    // Mostrar el resultado
    cout << total << endl;
}

// Promise & Future

void producer(promise<string> prm) {
    this_thread::sleep_for(1000ms);
    prm.set_value("Hola !!!");              // El promise envia el mensaje
    this_thread::sleep_for(1000ms);
    cout << "Fin del Producer\n";
}

void consumer(future<string> fut) {
    cout << fut.get() << endl;
}

void ejemplo_6() {
    promise<string> prm_1;
    future<string> fut_1 = prm_1.get_future();

    thread t2(producer, std::move(prm_1));
    cout << fut_1.get() << endl;            // El future lee el mensaje
    t2.join();
}

void ejemplo_7() {
    promise<string> prm_1;
    future<string> fut_1 = prm_1.get_future();
    thread t1(producer, std::move(prm_1));
    thread t2(consumer, std::move(fut_1));

    t1.join();
    t2.join();
}

int sumar_async(int a, int b) {
    return a + b;
}

void ejemplo_8() {
     auto resultado = async(sumar_async, 10, 20); // Resultado es un future
     cout << resultado.get();
}

int main() {
//    ejemplo1();
//    ejemplo2();
//    ejemplo3();
//    ejemplo4();
//    ejemplo5();
//    ejemplo_6();
//    ejemplo_7();
    ejemplo_8();
    return 0;
}
