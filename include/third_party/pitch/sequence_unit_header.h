//
// Created by phamanhtan on 29/5/25.
//

#ifndef SEQUENCE_UNIT_HEADER_H
#define SEQUENCE_UNIT_HEADER_H

#pragma once
#include "message.h"
#include <string>
#include <memory>
#include <stdexcept>

namespace CboePitch {
    class SeqUnitHeader : public Message {
    private:
        uint32_t hseq;
        std::string unit;
        std::string symbol;

    public:
        SeqUnitHeader(uint32_t seq, const std::string &u, const std::string &sym = "")
            : hseq(seq), unit(u), symbol(sym) {
            validate();
        }

        static std::unique_ptr<SeqUnitHeader> parse(const std::string &line) {
            if (line.size() < 10) {
                throw std::invalid_argument("Invalid SeqUnitHeader length");
            }
            try {
                uint32_t seq = std::stoul(line.substr(0, 8));
                std::string u = line.substr(8, 2);
                return std::make_unique<SeqUnitHeader>(seq, u);
            } catch (const std::exception &e) {
                throw std::invalid_argument("SeqUnitHeader parse error: " + std::string(e.what()));
            }
        }

        std::string getSymbol() const override { return symbol; }

        void setSymbol(const std::string &sym) override {
            if (sym.size() > 8) throw std::invalid_argument("Symbol must be <= 8 characters");
            symbol = sym;
        }

        char getMessageType() const override { return 'H'; }

        std::string toString() const override {
            char buffer[100];
            snprintf(buffer, sizeof(buffer),
                     "SeqUnitHeader: Sequence=%u, Unit=%s, Symbol=%s",
                     hseq, unit.c_str(), symbol.c_str());
            return std::string(buffer);
        }

    private:
        void validate() {
            if (unit.size() != 2) throw std::invalid_argument("Unit must be 2 characters");
            if (symbol.size() > 8) throw std::invalid_argument("Symbol must be <= 8 characters");
        }
    };
} // namespace CboePitch
#endif
