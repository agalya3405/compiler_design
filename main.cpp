#include <bits/stdc++.h>
using namespace std;

enum class TokenType { END, NUMBER, PLUS, MINUS, MUL, DIV, LPAREN, RPAREN };

struct Token {
    TokenType type;
    double value; // only used when type == NUMBER
    Token(TokenType t = TokenType::END, double v = 0.0) : type(t), value(v) {}
};

class Lexer {
    string s;
    size_t pos = 0;
public:
    Lexer(const string& input) : s(input), pos(0) {}

    Token nextToken() {
        while (pos < s.size() && isspace((unsigned char)s[pos])) ++pos;
        if (pos >= s.size()) return Token(TokenType::END);

        char c = s[pos];

        if (c == '+') { ++pos; return Token(TokenType::PLUS); }
        if (c == '-') { ++pos; return Token(TokenType::MINUS); }
        if (c == '*') { ++pos; return Token(TokenType::MUL); }
        if (c == '/') { ++pos; return Token(TokenType::DIV); }
        if (c == '(') { ++pos; return Token(TokenType::LPAREN); }
        if (c == ')') { ++pos; return Token(TokenType::RPAREN); }

        // Number: integer or decimal (supports leading dot like .5)
        if (isdigit((unsigned char)c) || c == '.') {
            size_t start = pos;
            bool dotSeen = (c == '.');
            ++pos;
            while (pos < s.size()) {
                char nc = s[pos];
                if (isdigit((unsigned char)nc)) { ++pos; continue; }
                if (nc == '.' && !dotSeen) { dotSeen = true; ++pos; continue; }
                break;
            }
            string numStr = s.substr(start, pos - start);
            try {
                double val = stod(numStr);
                return Token(TokenType::NUMBER, val);
            } catch (...) {
                throw runtime_error("Invalid number: " + numStr);
            }
        }

        throw runtime_error(string("Unknown character: '") + c + "'");
    }
};

class Parser {
    Lexer lexer;
    Token cur;

    void advance() { cur = lexer.nextToken(); }

    // Grammar:
    // Expression := Term { ('+' | '-') Term }
    // Term       := Factor { ('*' | '/') Factor }
    // Factor     := NUMBER | '(' Expression ')' | ('+' | '-') Factor

    double parseExpression() {
        double value = parseTerm();
        while (cur.type == TokenType::PLUS || cur.type == TokenType::MINUS) {
            TokenType op = cur.type;
            advance();
            double rhs = parseTerm();
            if (op == TokenType::PLUS) value += rhs;
            else value -= rhs;
        }
        return value;
    }

    double parseTerm() {
        double value = parseFactor();
        while (cur.type == TokenType::MUL || cur.type == TokenType::DIV) {
            TokenType op = cur.type;
            advance();
            double rhs = parseFactor();
            if (op == TokenType::MUL) value *= rhs;
            else {
                if (rhs == 0.0) throw runtime_error("Division by zero");
                value /= rhs;
            }
        }
        return value;
    }

    double parseFactor() {
        if (cur.type == TokenType::NUMBER) {
            double val = cur.value;
            advance();
            return val;
        }
        if (cur.type == TokenType::LPAREN) {
            advance(); // consume '('
            double val = parseExpression();
            if (cur.type != TokenType::RPAREN) throw runtime_error("Expected ')'");
            advance(); // consume ')'
            return val;
        }
        // Unary plus/minus
        if (cur.type == TokenType::PLUS) {
            advance();
            return parseFactor();
        }
        if (cur.type == TokenType::MINUS) {
            advance();
            return -parseFactor();
        }
        throw runtime_error("Unexpected token in factor");
    }

public:
    Parser(const string& input) : lexer(input) {
        advance(); // load first token
    }

    double parse() {
        double result = parseExpression();
        if (cur.type != TokenType::END) throw runtime_error("Unexpected input after expression");
        return result;
    }
};

int main() {
    cout << "Simple expression evaluator (type 'quit' or Ctrl+D to exit)\n";
    string line;
    while (true) {
        cout << ">>> ";
        if (!std::getline(cin, line)) break;
        if (line.size() == 0) continue;
        if (line == "quit" || line == "exit") break;

        try {
            Parser p(line);
            double result = p.parse();
            // Print result neatly: remove trailing .0 if integer
            long long asInt = (long long)result;
            if (fabs(result - (double)asInt) < 1e-12) cout << asInt << "\n";
            else cout << result << "\n";
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
    cout << "Bye.\n";
    return 0;
}
