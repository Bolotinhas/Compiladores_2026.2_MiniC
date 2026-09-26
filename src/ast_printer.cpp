#include "ast_printer.hpp"

#include <sstream>

namespace minicpp {

namespace {

const char* unary_name(UnaryOp op) {
    switch (op) {
        case UnaryOp::Neg: return "neg";
        case UnaryOp::Not: return "!";
        case UnaryOp::PreInc: return "pre++";
        case UnaryOp::PreDec: return "pre--";
        case UnaryOp::PostInc: return "post++";
        case UnaryOp::PostDec: return "post--";
    }
    return "?";
}

const char* binary_name(BinaryOp op) {
    switch (op) {
        case BinaryOp::Add: return "+";
        case BinaryOp::Sub: return "-";
        case BinaryOp::Mul: return "*";
        case BinaryOp::Div: return "/";
        case BinaryOp::Mod: return "%";
        case BinaryOp::Eq: return "==";
        case BinaryOp::Ne: return "!=";
        case BinaryOp::Lt: return "<";
        case BinaryOp::Le: return "<=";
        case BinaryOp::Gt: return ">";
        case BinaryOp::Ge: return ">=";
        case BinaryOp::And: return "&&";
        case BinaryOp::Or: return "||";
        case BinaryOp::Shl: return "<<";
    }
    return "?";
}

const char* assign_name(AssignOp op) {
    switch (op) {
        case AssignOp::Assign: return "=";
        case AssignOp::AddAssign: return "+=";
        case AssignOp::SubAssign: return "-=";
    }
    return "?";
}

std::string quote(const std::string& s) {
    std::string out = "\"";
    for (char c : s) {
        switch (c) {
            case '\n': out += "\\n"; break;
            case '\t': out += "\\t"; break;
            case '\r': out += "\\r"; break;
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            default: out += c;
        }
    }
    return out + "\"";
}

struct Printer {
    std::string operator()(const IntLit& n) const { return std::to_string(n.value); }

    std::string operator()(const DoubleLit& n) const {
        std::ostringstream os;
        os.precision(15);
        os << n.value;
        std::string s = os.str();
        // garante que 2.0 nao seja impresso como "2" (pareceria um inteiro)
        if (s.find_first_of(".eEn") == std::string::npos) s += ".0";
        return s;
    }

    std::string operator()(const StringLit& n) const { return quote(n.value); }
    std::string operator()(const BoolLit& n) const { return n.value ? "true" : "false"; }
    std::string operator()(const Ident& n) const { return n.name; }

    std::string operator()(const Unary& n) const {
        return std::string("(") + unary_name(n.op) + " " + to_sexpr(*n.operand) + ")";
    }
    std::string operator()(const Binary& n) const {
        return std::string("(") + binary_name(n.op) + " " + to_sexpr(*n.lhs) + " " +
               to_sexpr(*n.rhs) + ")";
    }
    std::string operator()(const Assign& n) const {
        return std::string("(") + assign_name(n.op) + " " + to_sexpr(*n.target) + " " +
               to_sexpr(*n.value) + ")";
    }
    std::string operator()(const Call& n) const {
        std::string out = "(call " + n.callee;
        for (const auto& arg : n.args) out += " " + to_sexpr(*arg);
        return out + ")";
    }
    std::string operator()(const Index& n) const {
        return "(index " + to_sexpr(*n.base) + " " + to_sexpr(*n.index) + ")";
    }
};

}  // namespace

std::string to_sexpr(const Expr& expr) { return std::visit(Printer{}, expr.node); }

}  // namespace minicpp
