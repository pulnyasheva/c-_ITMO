#include "calc.h"

#include <cctype>   // for std::isspace
#include <cmath>    // various math functions
#include <iostream> // for error reporting via std::cerr

namespace {

const std::size_t max_decimal_digits = 10;

enum class Op
{
    ERR,
    SET,
    ADD,
    SUB,
    MUL,
    DIV,
    REM,
    NEG,
    POW,
    SQRT
};

std::size_t arity(const Op op)
{
    switch (op) {
    // error
    case Op::ERR: return 0;
    // unary
    case Op::NEG: return 1;
    case Op::SQRT: return 1;
    // binary
    case Op::SET: return 2;
    case Op::ADD: return 2;
    case Op::SUB: return 2;
    case Op::MUL: return 2;
    case Op::DIV: return 2;
    case Op::REM: return 2;
    case Op::POW: return 2;
    }
    return 0;
}

struct several_op
{
    Op op;
    bool several_args;
};

several_op parse_op(const std::string & line, std::size_t & i)
{
    several_op this_op;
    this_op.several_args = false;
    const auto rollback = [&i, &line](const std::size_t n) {
        i -= n;
        std::cerr << "Unknown operation " << line << std::endl;
        several_op this_op;
        this_op.op = Op::ERR;
        return this_op;
    };
    if (line[i] == '(') {
        ++i;
        if (line[i + 1] == ')') {
            this_op.several_args = true;
        }
        else {
            std::cerr << "Need bracket: '" << line << "'" << std::endl;
            return rollback(1);
        }
    }
    switch (line[i++]) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        --i; // a first digit is a part of op's argument
        if (!this_op.several_args) {
            this_op.op = Op::SET;
            return this_op;
        }
        return rollback(1);
    case '+':
        this_op.op = Op::ADD;
        return this_op;
    case '-':
        this_op.op = Op::SUB;
        return this_op;
    case '*':
        this_op.op = Op::MUL;
        return this_op;
    case '/':
        this_op.op = Op::DIV;
        return this_op;
    case '%':
        this_op.op = Op::REM;
        return this_op;
    case '_':
        this_op.op = Op::NEG;
        return this_op;
    case '^':
        this_op.op = Op::POW;
        return this_op;
    case 'S':
        switch (line[i++]) {
        case 'Q':
            switch (line[i++]) {
            case 'R':
                switch (line[i++]) {
                case 'T':
                    this_op.op = Op::SQRT;
                    return this_op;
                default:
                    return rollback(4);
                }
            default:
                return rollback(3);
            }
        default:
            return rollback(2);
        }
    default:
        return rollback(1);
    }
}

std::size_t skip_ws(const std::string & line, std::size_t i)
{
    while (i < line.size() && std::isspace(line[i])) {
        ++i;
    }
    return i;
}

std::size_t skip_bracket(bool several_args, std::size_t & i)
{
    if (several_args) {
        return ++i;
    }
    return i;
}

double parse_arg(const std::string & line, std::size_t & i, bool several_args)
{
    double res = 0;
    std::size_t count = 0;
    bool good = true;
    bool integer = true;
    double fraction = 1;
    while (good && i < line.size() && count < max_decimal_digits) {
        if (several_args && std::isspace(line[i])) {
            return res;
        }
        switch (line[i]) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            if (integer) {
                res *= 10;
                res += line[i] - '0';
            }
            else {
                fraction /= 10;
                res += (line[i] - '0') * fraction;
            }
            ++i;
            ++count;
            break;
        case '.':
            integer = false;
            ++i;
            break;
        default:
            good = false;
            break;
        }
    }
    if (!good) {
        std::cerr << "Argument parsing error at " << i << ": '" << line.substr(i) << "'" << std::endl;
        return 0;
    }
    else if (i < line.size() && !several_args) {
        std::cerr << "Argument isn't fully parsed, suffix left: '" << line.substr(i) << "'" << std::endl;
        return 0;
    }
    return res;
}

double unary(const double current, const Op op)
{
    switch (op) {
    case Op::NEG:
        return -current;
    case Op::SQRT:
        if (current > 0) {
            return std::sqrt(current);
        }
        else {
            std::cerr << "Bad argument for SQRT: " << current << std::endl;
            [[fallthrough]];
        }
    default:
        return current;
    }
}

double binary(const Op op, const double left, const double right)
{
    switch (op) {
    case Op::SET:
        return right;
    case Op::ADD:
        return left + right;
    case Op::SUB:
        return left - right;
    case Op::MUL:
        return left * right;
    case Op::DIV:
        if (right != 0) {
            return left / right;
        }
        else {
            std::cerr << "Bad right argument for division: " << right << std::endl;
            return left;
        }
    case Op::REM:
        if (std::abs(right) != 0) {
            return std::fmod(left, right);
        }
        else {
            std::cerr << "Bad right argument for remainder: " << right << std::endl;
            return left;
        }
    case Op::POW:
        return std::pow(left, right);
    default:
        return left;
    }
}

} // anonymous namespace

double process_line(const double current, const std::string & line)
{
    std::size_t i = 0;
    bool error = false;
    double this_number = current;
    const auto this_op = parse_op(line, i);
    switch (arity(this_op.op)) {
    case 2: {
        do {
            i = skip_bracket(this_op.several_args, i);
            i = skip_ws(line, i);
            const auto old_i = i;
            const auto arg = parse_arg(line, i, this_op.several_args);
            if (i == old_i) {
                std::cerr << "No argument for a binary operation" << std::endl;
                break;
            }
            else if ((this_op.op == Op::REM || this_op.op == Op::DIV) && arg == 0) {
                error = true;
            }
            this_number = binary(this_op.op, this_number, arg);
        } while (i < line.size() && this_op.several_args);
        if (i < line.size() || error) {
            return current;
        }
        return this_number;
    }
    case 1: {
        if (i < line.size()) {
            std::cerr << "Unexpected suffix for a unary operation: '" << line.substr(i) << "'" << std::endl;
            break;
        }
        return unary(current, this_op.op);
    }
    default: break;
    }
    return current;
}
