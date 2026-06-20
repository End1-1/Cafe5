#pragma once

enum class PaymentStubResult {
  None = 0,
  Paid,
  InsufficientFunds,
  OtherError,
};
