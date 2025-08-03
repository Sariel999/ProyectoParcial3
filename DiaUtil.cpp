#include "DiaUtil.h"
#include "Fecha.h"


bool esMismaFecha(const Fecha& a, const Fecha& b) {
    return a.getDia() == b.getDia() && a.getMes() == b.getMes();
}

bool esNoTrasladable(int dia, int mes) {
    return (dia == 1 && mes == 1) ||   // Año nuevo
           (dia == 1 && mes == 5) ||   // Día del trabajo
           (dia == 25 && mes == 12);   // Navidad
}

Fecha trasladarFeriadoSegunLey(const Fecha& original) {
    int ds = calcularDiaSemana(original.getDia(), original.getMes(), original.getAnio().getAnio());
    Fecha trasladado = original;

    if (ds == 6) { // Sábado → viernes anterior
        trasladado = restarUnDia(original);
    } else if (ds == 0) { // Domingo → lunes siguiente
        trasladado = sumarUnDia(original);
    } else if (ds == 2 || ds == 3) { // Martes o Miércoles → lunes siguiente
        do {
            trasladado = sumarUnDia(trasladado);
        } while (calcularDiaSemana(trasladado.getDia(), trasladado.getMes(), trasladado.getAnio().getAnio()) != 1);
    } else if (ds == 4) { // Jueves → viernes inmediato
        do {
            trasladado = sumarUnDia(trasladado);
        } while (calcularDiaSemana(trasladado.getDia(), trasladado.getMes(), trasladado.getAnio().getAnio()) != 5);
    }

    return trasladado;
}

int calcularDiaSemana(int d, int m, int y) {
    if (m < 3) {
        m += 12;
        y--;
    }
    int k = y % 100;
    int j = y / 100;
    int h = (d + 13*(m + 1)/5 + k + k/4 + j/4 + 5*j) % 7;
    return ((h + 6) % 7); // 0=Domingo, 1=Lunes, ..., 6=Sábado
}

Fecha calcularPascua(int anio) {
    int a = anio % 19;
    int b = anio / 100;
    int c = anio % 100;
    int d = b / 4;
    int e = b % 4;
    int f = (b + 8) / 25;
    int g = (b - f + 1) / 3;
    int h = (19 * a + b - d - g + 15) % 30;
    int i = c / 4;
    int k = c % 4;
    int l = (32 + 2 * e + 2 * i - h - k) % 7;
    int m = (a + 11 * h + 22 * l) / 451;
    int mes = (h + l - 7 * m + 114) / 31;
    int dia = ((h + l - 7 * m + 114) % 31) + 1;

    Fecha fecha;
    Anio aObj; aObj.setAnio(anio);
    fecha.setAnio(aObj);
    fecha.setMes(mes);
    fecha.setDia(dia);
    return fecha;
}

std::vector<Fecha> feriadosMoviles(int anio) {
    std::vector<Fecha> moviles;
    Fecha pascua = calcularPascua(anio);

    Fecha lunesCarnaval = pascua;
    Fecha martesCarnaval = pascua;
    for (int i = 0; i < 48; ++i) lunesCarnaval = restarUnDia(lunesCarnaval);
    for (int i = 0; i < 47; ++i) martesCarnaval = restarUnDia(martesCarnaval);

    Fecha viernesSanto = pascua;
    for (int i = 0; i < 2; ++i) viernesSanto = restarUnDia(viernesSanto);

    moviles.push_back(lunesCarnaval);
    moviles.push_back(martesCarnaval);
    moviles.push_back(viernesSanto);

    return moviles;
}

std::vector<Fecha> obtenerFeriadosFijos(int anio) {
    std::vector<Fecha> feriados;
    int fijos[][2] = {
        {1, 1}, {1, 5}, {24, 5}, {10, 8},
        {9, 10}, {2, 11}, {3, 11}, {25, 12}
    };

    for (auto& f : fijos) {
        Fecha fecha;
        Anio a; a.setAnio(anio);
        fecha.setAnio(a);
        fecha.setDia(f[0]);
        fecha.setMes(f[1]);

        if (!esNoTrasladable(f[0], f[1])) {
            fecha = trasladarFeriadoSegunLey(fecha);
        }

        feriados.push_back(fecha);
    }

    return feriados;
}

bool esFeriado(const Fecha& fecha) {
    int anio = fecha.getAnio().getAnio();
    auto fijos = obtenerFeriadosFijos(anio);
    auto moviles = feriadosMoviles(anio);

    for (const auto& f : fijos)
        if (esMismaFecha(fecha, f)) return true;

    for (const auto& f : moviles)
        if (esMismaFecha(fecha, f)) return true;

    return false;
}

bool esDiaLaborable(const Fecha& fecha) {
    int ds = calcularDiaSemana(fecha.getDia(), fecha.getMes(), fecha.getAnio().getAnio());
    return ds >= 1 && ds <= 5 && !esFeriado(fecha);
}

Fecha sumarUnDia(Fecha f) {
    int dia = f.getDia(), mes = f.getMes(), anio = f.getAnio().getAnio();
    int diamax = 0;

    switch (mes) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12: diamax = 31; break;
        case 4: case 6: case 9: case 11: diamax = 30; break;
        case 2: diamax = (f.getAnio().getAnioBisiesto() ? 29 : 28); break;
    }

    dia++;
    if (dia > diamax) {
        dia = 1;
        mes++;
        if (mes > 12) {
            mes = 1;
            anio++;
        }
    }

    f.setDia(dia);
    f.setMes(mes);
    Anio a; a.setAnio(anio); f.setAnio(a);
    return f;
}

Fecha restarUnDia(Fecha f) {
    int dia = f.getDia(), mes = f.getMes(), anio = f.getAnio().getAnio();

    dia--;
    if (dia < 1) {
        mes--;
        if (mes < 1) {
            mes = 12;
            anio--;
        }

        switch (mes) {
            case 1: case 3: case 5: case 7: case 8: case 10: case 12: dia = 31; break;
            case 4: case 6: case 9: case 11: dia = 30; break;
            case 2: {
                Anio aTemp; aTemp.setAnio(anio);
                dia = (aTemp.getAnioBisiesto() ? 29 : 28);
                break;
            }
        }
    }

    f.setDia(dia);
    f.setMes(mes);
    Anio a; a.setAnio(anio); f.setAnio(a);
    return f;
}

Fecha ajustarAlDiaHabilMasCercano(Fecha f) {
    if (esDiaLaborable(f)) return f;

    int ds = calcularDiaSemana(f.getDia(), f.getMes(), f.getAnio().getAnio());

    if (ds == 6) { // Sábado
        Fecha anterior = restarUnDia(f);
        if (esDiaLaborable(anterior)) return anterior;
    }

    // Buscar siguiente laborable
    while (!esDiaLaborable(f)) {
        f = sumarUnDia(f);
    }

    return f;
}
/*
Fecha siguienteFeriadoDesde(Fecha f) {
    // Paso 1: encontrar el próximo feriado después de la fecha ingresada
    Fecha siguienteFeriado = siguienteFeriadoDesde(f);

    // Paso 2: desde ese feriado, buscar el siguiente día laborable
    Fecha diaLaboral = ajustarAlDiaHabilMasCercano(siguienteFeriado);
}
*/