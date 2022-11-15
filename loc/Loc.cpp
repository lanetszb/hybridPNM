/* MIT License
 *
 * Copyright (c) 2022 Aleksandr Zhuravlyov and Zakhar Lanets
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include "Loc.h"

using namespace hybridPNM;


Loc::Loc(netgrid::Netgrid &netgrid, Props &props,
         Eigen::Map<Eigen::VectorXd> u,
         Eigen::Map<Eigen::VectorXd> dS0,
         std::vector<Eigen::Map<Eigen::VectorXd>> P,
         std::vector<Eigen::Map<Eigen::VectorXd>> S0,
         std::map<std::string, Eigen::Map<Eigen::VectorXd>> data)
    : Variables(0, 1, u, dS0, P, S0, data), _netgrid(netgrid), _props(props),
      _dV(std::vector<double>(_netgrid._cellsN, 0)),
      _free(std::vector<double>(_netgrid._cellsN, 0)),
      _element(std::vector<double>(_netgrid._cellsN, 0)) {

  for (auto &[throat, cells]: _netgrid._throatsCells)
    for (auto &cell: cells)
      _dV[cell] = _netgrid._throatsDVs[throat];

}


Loc::Loc(netgrid::Netgrid &netgrid, Props &props,
         Eigen::Ref<Eigen::VectorXd> u,
         Eigen::Ref<Eigen::VectorXd> dS0,
         std::vector<Eigen::Ref<Eigen::VectorXd>> P,
         std::vector<Eigen::Ref<Eigen::VectorXd>> S0,
         std::map<std::string, Eigen::Ref<Eigen::VectorXd>> data) :
    Loc(netgrid, props,
        setter<Eigen::VectorXd>(u),
        setter<Eigen::VectorXd>(dS0),
        vectorSetter<Eigen::VectorXd>(P),
        vectorSetter<Eigen::VectorXd>(S0),
        mapSetter<std::string, Eigen::VectorXd>(data)) {}


Loc::~Loc() {}


void Loc::iterateIndices() {
  std::swap(_iPrev, _iCurr);
}


void Loc::imposeBcDirichlet() {

  for (auto &[name, value]: _bc)
    for (auto &face: _netgrid._typesFaces[name])
      for (auto &cell: _netgrid._neighborsCells[face]) {
        _free[cell] = -_bc[name];
        _element[cell] = 1;
      }
}
