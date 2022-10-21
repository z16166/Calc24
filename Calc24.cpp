#include <assert.h>
#include <boost/rational.hpp>
#include <format>
#include <set>

using rational = boost::rational<int>;

int d1, d2, d3, d4; // 4个操作数
char o1, o2, o3;    // 3个运算符
int p1, p2, p3;     // 3个运算符的优先级

std::set<std::string> solutions; // 消重用的。没彻底消重

// 执行真正的加减乘除操作
bool Operation(const rational &a, const char op, const rational &b,
               rational &r) {
  switch (op) {
  case '+':
    r = a + b;
    return true;
  case '-':
    if (a >= b) {
      r = a - b;
      return true;
    }
    return false;
  case '*':
    r = a * b;
    return true;
  case '/':
    if (b == 0)
      return false;
    r = a / b;
    return true;
  }

  return false;
}

// 比较加减乘除操作符的优先级，加减的优先级低于乘除的优先级。
int CompareOp(const char leftOp, const char rightOp) {
  const int leftLevel = (leftOp == '+' || leftOp == '-') ? 1 : 2;
  const int rightLevel = (rightOp == '+' || rightOp == '-') ? 1 : 2;

  if (leftLevel > rightLevel)
    return 1;
  else if (leftLevel < rightLevel)
    return -1;
  else
    return 0;
}

// 根据位置获取3个操作符中指定的那个
const char GetOp(const int pos) {
  assert(pos >= 1 && pos <= 3);

  switch (pos) {
  case 1:
    return o1;
  case 2:
    return o2;
  case 3:
    return o3;
  }

  return 0;
}

// 获取当前操作符左侧的操作数
const int GetLeftData(const int pos) {
  switch (pos) {
  case 1:
    return d1;
  case 2:
    return d2;
  case 3:
    return d3;
  default:
    assert(false);
  }

  return 0;
}

// 获取当前操作符右侧的操作数
const int GetRightData(const int pos) {
  switch (pos) {
  case 1:
    return d2;
  case 2:
    return d3;
  case 3:
    return d4;
  default:
    assert(false);
  }

  return 0;
}

std::string Format(const int pos, const int nextPos, const int prevPos,
                   const std::string &prevFmt) {
  // 是否需要加括号。原则是尽量不加括号。
  // 根据下一个运算符的优先级，以及下一个运算符是在当前运算符的左侧还是右侧决定的。
  bool needBracket = false;

  // 当前运算符
  const char myOp = GetOp(pos);

  // 下一个运算符
  if (nextPos != -1) {
    // 下一个运算符在左侧。比较优先级，确定是否要加括号。
    // 优先级相等时，如果左侧是减号或者除号，则当前必须加括号。
    if (nextPos < pos) {
      const char leftOp = GetOp(nextPos);
      const auto cmp = CompareOp(leftOp, myOp);
      if (cmp > 0 || (cmp == 0 && (leftOp == '-' || leftOp == '/'))) {
        needBracket = true;
      }
    }

    // 下一个运算符在右侧。比较优先级，确定是否要加括号
    if (nextPos > pos) {
      const char rightOp = GetOp(nextPos);
      if (CompareOp(myOp, rightOp) < 0) {
        needBracket = true;
      }
    }
  }

  constexpr auto fmt = "{} {} {}";

  std::string result;

  // 确定当前运算符左右两侧的数据是什么，并生成当前表达式
  if (prevPos == -1) {
    result = std::format(fmt, GetLeftData(pos), myOp, GetRightData(pos));
  } else {
    if (pos < prevPos) {
      result = std::format(fmt, GetLeftData(pos), myOp, prevFmt);
    } else {
      result = std::format(fmt, prevFmt, myOp, GetRightData(pos));
    }
  }

  // 按需加括号
  return needBracket ? "(" + result + ")" : result;
}

bool FormatResult(const int pos1, const int pos2, const int pos3) {
  std::string fmt;

  // 先算两边、再算中间这种情况，要特殊处理一下。
  if ((pos1 == 3 && pos2 == 1) || (pos1 == 1 && pos2 == 3)) {
    const auto left = Format(1, 2, -1, "");
    const auto right = Format(3, 2, -1, "");
    fmt = std::format("{} {} {}", left, o2, right);
  } else {
    // 依次计算pos1、pos2、pos3指向的操作符。
    fmt = Format(pos1, pos2, -1, "");
    fmt = Format(pos2, pos3, pos1, fmt);
    fmt = Format(pos3, -1, pos2, fmt);
  }

  // 简单消重
  if (solutions.find(fmt) == solutions.end()) {
    printf("%s = 24\n", fmt.c_str());
    solutions.insert(fmt);
    return true;
  }

  return false;
}

// 对穷举出来的一种状态，判断是否能计算出24点。
bool IsValid() {
  if (p1 == 3) {
    rational t1;
    if (!Operation(d1, o1, d2, t1))
      return false;

    if (p2 == 2) {
      rational t2;
      if (!Operation(t1, o2, d3, t2))
        return false;

      rational r;
      if (!Operation(t2, o3, d4, r))
        return false;

      if (r == 24) {
        return FormatResult(1, 2, 3);
      }
    } else {
      rational t3;
      if (!Operation(d3, o3, d4, t3))
        return false;

      rational r;
      if (!Operation(t1, o2, t3, r))
        return false;

      if (r == 24) {
        return FormatResult(1, 3, 2);
      }
    }
  } else if (p2 == 3) {
    rational t2;
    if (!Operation(d2, o2, d3, t2))
      return false;

    if (p1 == 2) {
      rational t1;
      if (!Operation(d1, o1, t2, t1))
        return false;

      rational r;
      if (!Operation(t1, o3, d4, r))
        return false;

      if (r == 24) {
        return FormatResult(2, 1, 3);
      }
    } else {
      rational t3;
      if (!Operation(t2, o3, d4, t3))
        return false;

      rational r;
      if (!Operation(d1, o1, t3, r))
        return false;

      if (r == 24) {
        return FormatResult(2, 3, 1);
      }
    }
  } else if (p3 == 3) {
    rational t3;
    if (!Operation(d3, o3, d4, t3))
      return false;

    if (p1 == 2) {
      rational t1;
      if (!Operation(d1, o1, d2, t1))
        return false;

      rational r;
      if (!Operation(t1, o2, t3, r))
        return false;

      if (r == 24) {
        return FormatResult(3, 1, 2);
      }
    } else {
      rational t2;
      if (!Operation(d2, o2, t3, t2))
        return false;

      rational r;
      if (!Operation(d1, o1, t2, r))
        return false;

      if (r == 24) {
        return FormatResult(3, 2, 1);
      }
    }
  }

  return false;
}

void Calc24(const int a, const int b, const int c, const int d) {
  const int data[] = {a, b, c, d}; // 操作数。每次用这4个得到一个不重复的排列。
  const int op[] = {'+', '-', '*', '/'}; // 操作符。每次从4个中选出可重复的3个。
  const int pri[] = {1, 2, 3}; // 优先级。每次用这3个得到一个不重复的排列。

  solutions.clear();

  // k1到k4，用来得到4个操作数的一个不重复的排列。
  for (int k1 = 0; k1 < 4; ++k1) {
    d1 = data[k1];

    for (int k2 = 0; k2 < 4; ++k2) {
      if (k2 == k1)
        continue;

      d2 = data[k2];

      for (int k3 = 0; k3 < 4; ++k3) {
        if (k3 == k2 || k3 == k1)
          continue;

        d3 = data[k3];
        for (int k4 = 0; k4 < 4; ++k4) {
          if (k4 == k3 || k4 == k2 || k4 == k1)
            continue;

          d4 = data[k4];

          // m1到m3，用来得到3个运算符的一个可重复排列。
          for (int m1 = 0; m1 < 4; ++m1) {
            o1 = op[m1];

            for (int m2 = 0; m2 < 4; ++m2) {
              o2 = op[m2];

              for (int m3 = 0; m3 < 4; ++m3) {
                o3 = op[m3];

                // n1到n3，用来得到3个运算符优先级的一个不重复的排列。
                for (int n1 = 0; n1 < 3; ++n1) {
                  p1 = pri[n1];

                  for (int n2 = 0; n2 < 3; ++n2) {
                    if (n2 == n1)
                      continue;

                    p2 = pri[n2];

                    for (int n3 = 0; n3 < 3; ++n3) {
                      if (n3 == n2 || n3 == n1)
                        continue;

                      p3 = pri[n3];

                      if (IsValid()) {
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  printf("\n");
}

int wmain(int argc, wchar_t **argv) {
  if (argc >= 5) {
    int a[4];
    for (int k = 0; k < 4; ++k) {
      a[k] = _wtoi(argv[k + 1]);
    }

    Calc24(a[0], a[1], a[2], a[3]);
    return 0;
  }

  Calc24(1, 2, 3, 4);

  Calc24(11, 12, 13, 14);

  Calc24(5, 5, 5, 1);
  Calc24(3, 3, 8, 8);
  Calc24(12, 2, 9, 10);
  Calc24(6, 6, 13, 17);
  Calc24(4, 4, 10, 10);
  return 0;
}