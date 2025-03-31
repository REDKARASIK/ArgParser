#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace ArgumentParser {

    template <typename T>
    class Argument {
    public:
        Argument() = default;
        explicit Argument(const std::string& short_arg, const std::string& long_arg, const std::string& information_of_arg="") 
            : short_arg_(short_arg), 
            long_arg_(long_arg), 
            information_of_arg_(information_of_arg) {
                if (std::is_same<T, bool>::value) {
                    value_ = false;
                }
            }

        ~Argument(){};

        Argument<T>& Default(const T& value);
        Argument<T>& Default(const std::vector<T>& values);
        void StoreValue(T& store_value);
        void StoreValues(std::vector<T>& store_values);
        T GetValue() const;
        T GetValue(int index) const;
        Argument<T>& MultiValue(int mmin_quantity_arg_values = 0);
        Argument<T>& Positional();
        bool GetPositional() const;
        void SetValue(const T& value);
        bool IsValueSet();
        std::string GetInformation() const;
        T GetDefaultValue() const;
        bool IsDefaultSet() const;
        bool IsMultiValue() const;
        int GetMinArgsQuantity() const;
        std::string GetShortArg() const;
        bool GetValueSet() const;

    private:
        const std::string short_arg_;
        const std::string long_arg_;
        const std::string information_of_arg_;
        bool is_multi_values_ = false;
        bool is_positional_ = false;
        bool is_value_set_ = false;
        bool is_default_set_ = false;
        int min_quantity_arg_values_;
        std::vector<T> values_;
        std::vector<T> default_values_;
        T value_;
        T default_value_;
        T* store_value_ = nullptr;
        std::vector<T>* store_values_ = nullptr;

        void ChangeValuesForStoreVariables() {
            if (store_value_ != nullptr && !is_multi_values_) {
                *store_value_ = value_;
            }
            if (store_values_ != nullptr && is_multi_values_) {
                if (is_default_set_ && values_.size() == 0) {
                    *store_values_ = default_values_;
                } else {
                    *store_values_ = values_;
                }
            }
        }
    };
    
    template <typename T>
    Argument<T>& Argument<T>::Default(const T& value) {
        if (is_multi_values_) {
            throw std::runtime_error("Incorrect. the Deafult value needs to be std::vector<T> of values");
        }
        default_value_ = value;
        is_default_set_ = true;
        if (std::is_same<T, bool>::value) {
            ChangeValuesForStoreVariables();
        }
        return *this;
    }

    template <typename T>
    Argument<T>& Argument<T>::Default(const std::vector<T>& values) {
        if (!is_multi_values_) {
            throw std::runtime_error("Can't be std::vector<T> of values for Default argument value");
        }
        if (values.size() < min_quantity_arg_values_) {
            throw std::runtime_error("Not enough values for multi argument. Need: " + std::to_string(min_quantity_arg_values_) + " Get: " + std::to_string(values.size()));
        }
        is_default_set_ = true;
        default_values_ = values;
        return *this;
    }
    
    template <typename T>
    void Argument<T>::StoreValue(T& store_value) {
        if (is_multi_values_) {
            throw std::runtime_error("Can't store value to not std::vector<T> variable");
        }
        store_value_ = &store_value;
    }

    template <typename T>
    void Argument<T>::StoreValues(std::vector<T>& store_values) {
        if (!is_multi_values_) {
            throw std::runtime_error("Can't strore value to std::vector<T>, please use not array type for variable");
        }
        store_values_ = &store_values;
    }

    template <typename T>
    T Argument<T>::GetValue() const {
        if (is_multi_values_) {
            throw std::runtime_error("Can't get value from multi argument without index");
        }
        if (is_value_set_) {
            return value_;
        }
        return default_value_;
    }

    template <typename T>
    T Argument<T>::GetValue(int index) const {
        if (!is_multi_values_) {
            throw std::runtime_error("Can't get value by index from not multi argument");
        }
        if (index < 0 || index > values_.size() - 1) {
            throw std::runtime_error("segmentation fault");
        }
        if (is_value_set_) {
            return values_[index];
        }
        return default_values_[index];
    }
    
    template<typename T>
    Argument<T>& Argument<T>::MultiValue(int min_quantity_arg_values) {
        if (std::is_same<T, bool>::value) {
            throw std::runtime_error("Flag argument can't be multi value");
        }
        min_quantity_arg_values_ = min_quantity_arg_values;
        is_multi_values_ = true;
        is_default_set_ = false;
        return *this;
    }

    template <typename T>
    Argument<T>& Argument<T>::Positional() {
        if (std::is_same<T, bool>::value) {
            throw std::runtime_error("Flag argument can't be positional");
        }
        is_positional_ = true;
        return *this;
    }

    template <typename T>
    bool Argument<T>::GetPositional() const {
        return is_positional_;
    }

    template <typename T>
    void Argument<T>::SetValue(const T& value) {
        is_value_set_ = true;
        if (is_multi_values_) {
            values_.push_back(value);
        } else {
            value_ = value;
        }
        if (std::is_same<T, bool>::value) {
            ChangeValuesForStoreVariables();
        }
    }

    template <typename T>
    bool Argument<T>::IsValueSet() {
        if (!is_multi_values_) {
            if (is_value_set_ || is_default_set_) {
                ChangeValuesForStoreVariables();
            }
            return is_value_set_;
        } else if ((values_.size() >= min_quantity_arg_values_) || is_default_set_) {
            ChangeValuesForStoreVariables();
        }
        return (values_.size() >= min_quantity_arg_values_);
    }

    template <typename T>
    std::string Argument<T>::GetInformation() const {
        return information_of_arg_;
    }

    template <typename T>
    bool Argument<T>::IsDefaultSet() const {
        return is_default_set_;
    }

    template <typename T>
    T Argument<T>::GetDefaultValue() const {
        if (!is_multi_values_) {
            return default_value_;
        }
        throw std::runtime_error("Incorrect type for getting default value for multi argument: " + long_arg_);
    }

    template <typename T>
    bool Argument<T>::IsMultiValue() const {
        return is_multi_values_;
    }

    template <typename T>
    int Argument<T>::GetMinArgsQuantity() const {
        return min_quantity_arg_values_;
    }

    template <typename T>
    std::string Argument<T>::GetShortArg() const {
        return short_arg_;
    }

    template <typename T>
    bool Argument<T>::GetValueSet() const {
        return is_value_set_;
    }
}
