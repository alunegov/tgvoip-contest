#include <rate-math.h>

#include <cassert>
#include <cmath>

bool Cfft(RateFloat_t *x, RateFloat_t *y, uint32_t n, bool aDirect)
{
    assert(x != nullptr);
    assert(y != nullptr);
    assert(n >= 2);

    if ((x == nullptr) || (y == nullptr) || (n < 2)) {
        return false;
    }

    uint32_t e, f, i, i1, j, j1, k, l, m, o;
    RateFloat_t c, p, q, r, s, t, u, v, w, z;

    m = FloorLog2(n);
    assert(n == PowOf2(m));
    for (l = 1; l <= m; ++l) {
        e = 1 << (m + 1 - l);
        f = e / 2;
        u = 1;
        v = 0;
        z = static_cast<RateFloat_t>(M_PI) / f;
        c = std::cos(z);
        s = std::sin(z);
        if (!aDirect) {
            s *= -1;
        }
        for (j = 1; j <= f; ++j) {
            for (i = j; i <= n; i += e) {
                i1 = i - 1;
                o = i1 + f;
                p = x[i1] + x[o];
                q = y[i1] + y[o];
                r = x[i1] - x[o];
                t = y[i1] - y[o];
                x[o] = r*u - t*v;
                y[o] = t*u + r*v;
                x[i1] = p;
                y[i1] = q;
            }
            w = u*c - v*s;
            v = v*c + u*s;
            u = w;
        }
    }

    // инверси€ битов: 
    // 000000001 -> 100000000
    // 000000010 -> 010000000
    // 000000011 -> 110000000
    // ......................
    j = 1;
    for (i = 1; i <= n - 1; ++i) {
        if (i < j) {
            j1 = j - 1;
            i1 = i - 1;
            p = x[j1];
            q = y[j1];
            x[j1] = x[i1];
            y[j1] = y[i1];
            x[i1] = p;
            y[i1] = q;
        }
        k = n / 2;
        while (k < j) {
            j -= k;
            k /= 2;
        }
        j += k;
    }

    if (aDirect) {
        for (k = 0; k < n; ++k) {
            x[k] /= n;
            y[k] /= n;
        }
    }

    // удал€ем посто€нную составл€ющую
    //x[0] = y[0] = 0.0;

    return true;
}
