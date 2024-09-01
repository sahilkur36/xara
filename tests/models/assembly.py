from math import cos,sin,sqrt,pi
import opensees as ops
model.node(201, 11758.709, 31062.549, 140.750*ft)
model.node(202, 11758.709, 31062.549, 192.776*ft)
model.node(203, 11758.709, 31062.549, 196.770*ft)
model.node(204, 11675.713, 30944.873, 197.490*ft)
model.node(205, 11571.969, 30797.777, 198.390*ft)
model.node(206, 11571.969, 30797.777, 194.397*ft)
model.node(207, 11571.969, 30797.777, 140.750*ft)
model.node(301, 13326.879, 30097.235, 140.000*ft)
model.node(302, 13326.879, 30097.235, 195.330*ft)
model.node(303, 13326.879, 30097.235, 199.570*ft)
model.node(304, 13227.933, 29932.695, 200.530*ft)
model.node(305, 12935.732, 29446.785, 203.370*ft)
model.node(306, 12935.732, 29446.785, 199.070*ft)
model.node(307, 12935.732, 29446.785, 126.000*ft)
model.node(401, 15603.570, 29883.836, 137.000*ft)
model.node(402, 15603.570, 29883.836, 189.910*ft)
model.node(403, 15603.570, 29883.836, 197.700*ft)
model.node(404, 15120.738, 28901.035, 203.145*ft)
model.node(405, 14983.164, 28621.004, 204.705*ft)
model.node(406, 14983.164, 28621.004, 196.930*ft)
model.node(407, 14983.164, 28621.004, 119.000*ft)
model.node(501, 17372.788, 28118.938, 131.000*ft)
model.node(502, 17372.788, 28118.938, 198.740*ft)
model.node(503, 17372.788, 28118.938, 204.004*ft)
model.node(504, 17308.901, 27950.657, 204.904*ft)
model.node(505, 17040.574, 27243.878, 208.684*ft)
model.node(506, 17040.574, 27243.878, 203.420*ft)
model.node(507, 17040.574, 27243.878, 138.000*ft)
model.node(601, 19284.324, 27591.751, 142.000*ft)
model.node(602, 19284.324, 27591.751, 199.780*ft)
model.node(603, 19284.324, 27591.751, 204.020*ft)
model.node(604, 19203.553, 27315.309, 205.460*ft)
model.node(605, 19047.059, 26779.703, 208.250*ft)
model.node(606, 19047.059, 26779.703, 204.010*ft)
model.node(607, 19047.059, 26779.703, 144.000*ft)
model.node(701, 20759.780, 27596.583, 144.000*ft)
model.node(702, 20759.780, 27596.583, 198.470*ft)
model.node(703, 20759.780, 27596.583, 202.334*ft)
model.node(704, 20609.812, 26947.687, 205.664*ft)
model.node(705, 20571.982, 26784.002, 206.504*ft)
model.node(706, 20571.982, 26784.002, 202.650*ft)
model.node(707, 20571.982, 26784.002, 149.500*ft)
model.node(801, 22434.979, 27020.686, 145.000*ft)
model.node(802, 22434.979, 27020.686, 199.690*ft)
model.node(803, 22434.979, 27020.686, 203.211*ft)
model.node(804, 22369.455, 26605.829, 205.311*ft)
model.node(805, 22332.013, 26368.768, 206.511*ft)
model.node(806, 22332.013, 26368.768, 202.990*ft)
model.node(807, 22332.013, 26368.768, 154.000*ft)
model.node(901, 24104.380, 26573.310, 136.000*ft)
model.node(902, 24104.380, 26573.310, 199.000*ft)
model.node(903, 24104.380, 26573.310, 203.028*ft)
model.node(904, 24088.125, 26390.192, 203.947*ft)
model.node(905, 24038.586, 25832.224, 206.748*ft)
model.node(906, 24038.586, 25832.224, 202.720*ft)
model.node(907, 24038.586, 25832.224, 156.000*ft)
model.node(1001, 25885.617, 26694.425, 131.000*ft)
model.node(1002, 25885.617, 26694.425, 194.810*ft)
model.node(1003, 25885.617, 26694.425, 199.852*ft)
model.node(1004, 25877.420, 26241.067, 202.119*ft)
model.node(1005, 25872.332, 25854.531, 204.052*ft)
model.node(1006, 25872.332, 25854.531, 199.010*ft)
model.node(1007, 25872.332, 25854.531, 130.000*ft)
model.node(1101, 27396.787, 26784.776, 141.500*ft)
model.node(1102, 27396.787, 26784.776, 192.090*ft)
model.node(1103, 27396.787, 26784.776, 197.154*ft)
model.node(1104, 27421.866, 26193.697, 200.112*ft)
model.node(1105, 27432.903, 25933.542, 201.414*ft)
model.node(1106, 27432.903, 25933.542, 196.350*ft)
model.node(1107, 27432.903, 25933.542, 139.000*ft)
model.node(1201, 29283.281, 26610.345, 166.000*ft)
model.node(1202, 29283.281, 26610.345, 192.159*ft)
model.node(1203, 29283.281, 26610.345, 195.409*ft)
model.node(1204, 29300.043, 26467.348, 195.903*ft)
model.node(1205, 29326.939, 26237.895, 196.695*ft)
model.node(1206, 29372.201, 25851.750, 198.028*ft)
model.node(1207, 29370.596, 25865.445, 197.981*ft)
model.node(1208, 29370.596, 25865.445, 193.445*ft)
model.node(1209, 29370.596, 25865.445, 156.000*ft)
model.node(1210, 29326.939, 26237.895, 194.731*ft)
model.node(1211, 29326.939, 26237.895, 159.000*ft)
model.node(1301, 30813.126, 26806.601, 163.750*ft)
model.node(1302, 30813.126, 26806.601, 189.331*ft)
model.node(1303, 30813.126, 26806.601, 192.581*ft)
model.node(1304, 30749.330, 26638.286, 193.022*ft)
model.node(1305, 30685.534, 26469.971, 193.463*ft)
model.node(1306, 30685.534, 26469.971, 190.215*ft)
model.node(1307, 30685.534, 26469.971, 161.750*ft)
model.node(1313, 30439.057, 25819.668, 151.000*ft)
model.node(1314, 30439.057, 25819.668, 193.333*ft)
model.node(1315, 30439.057, 25819.668, 196.584*ft)
model.node(1401, 32390.923, 27030.687, 158.500*ft)
model.node(1402, 32390.923, 27030.687, 186.803*ft)
model.node(1403, 32390.923, 27030.687, 190.053*ft)
model.node(1404, 32222.481, 26812.047, 190.352*ft)
model.node(1405, 32054.040, 26593.407, 190.651*ft)
model.node(1406, 32054.040, 26593.407, 187.402*ft)
model.node(1407, 32054.040, 26593.407, 158.500*ft)
model.node(1408, 32222.481, 26812.047, 187.102*ft)
model.node(1409, 32222.481, 26812.047, 158.500*ft)
model.node(1413, 31434.869, 25789.667, 150.500*ft)
model.node(1414, 31434.869, 25789.667, 193.182*ft)
model.node(1415, 31434.869, 25789.667, 196.436*ft)
model.node(10001, 10956.246, 31471.424, 195.779*ft)
model.node(10002, 11134.376, 31337.427, 196.224*ft)
model.node(10003, 11313.673, 31204.996, 196.658*ft)
model.node(10004, 11494.123, 31074.141, 197.082*ft)
model.node(20001, 11980.192, 30733.432, 198.159*ft)
model.node(20002, 12287.718, 30526.451, 198.794*ft)
model.node(20003, 12598.228, 30323.972, 199.401*ft)
model.node(20004, 12911.655, 30126.039, 199.978*ft)
model.node(30001, 13599.389, 29713.586, 201.129*ft)
model.node(30002, 13974.506, 29500.805, 201.691*ft)
model.node(30003, 14353.175, 29294.414, 202.215*ft)
model.node(30004, 14735.289, 29094.471, 202.700*ft)
model.node(40001, 15551.083, 28694.603, 203.593*ft)
model.node(40002, 15985.225, 28496.280, 203.992*ft)
model.node(40003, 16423.012, 28306.138, 204.344*ft)
model.node(40004, 16864.289, 28124.242, 204.648*ft)
model.node(50001, 17604.098, 27840.823, 205.081*ft)
model.node(50002, 18060.724, 27678.668, 205.225*ft)
model.node(50003, 18439.748, 27551.567, 205.335*ft)
model.node(50004, 18820.722, 27430.437, 205.412*ft)
model.node(60001, 19483.087, 27235.365, 205.533*ft)
model.node(60002, 19763.517, 27158.622, 205.592*ft)
model.node(60003, 20044.806, 27085.088, 205.634*ft)
model.node(60004, 20326.916, 27014.773, 205.658*ft)
model.node(70001, 20959.743, 26869.407, 205.648*ft)
model.node(70002, 21310.743, 26796.068, 205.604*ft)
model.node(70003, 21662.742, 26727.682, 205.534*ft)
model.node(70004, 22015.669, 26664.265, 205.436*ft)
model.node(80001, 22712.048, 26554.108, 205.085*ft)
model.node(80002, 23122.231, 26498.529, 204.834*ft)
model.node(80003, 23399.187, 26464.685, 204.557*ft)
model.node(80004, 23743.626, 26427.172, 204.258*ft)
model.node(90001, 24446.148, 26360.362, 203.584*ft)
model.node(90002, 24804.172, 26330.532, 203.220*ft)
model.node(90003, 25162.197, 26300.702, 202.854*ft)
model.node(90004, 25520.222, 26270.872, 202.486*ft)
model.node(100001, 26186.309, 26231.593, 201.719*ft)
model.node(100002, 26495.198, 26222.119, 201.319*ft)
model.node(100003, 26804.087, 26212.645, 200.917*ft)
model.node(100004, 27112.976, 26203.171, 200.515*ft)
model.node(110001, 27802.880, 26202.537, 199.488*ft)
model.node(110002, 28183.895, 26211.376, 198.835*ft)
model.node(110003, 28564.909, 26220.216, 198.143*ft)
model.node(110004, 28945.924, 26229.055, 197.397*ft)
model.node(120001, 29526.646, 26494.061, 195.445*ft)
model.node(120002, 29879.758, 26535.723, 194.631*ft)
model.node(120003, 30169.615, 26569.911, 193.995*ft)
model.node(120004, 30459.472, 26604.098, 193.360*ft)
model.node(120005, 29603.224, 25845.334, 197.570*ft)
model.node(120006, 29798.944, 25838.917, 197.342*ft)
model.node(120007, 30012.315, 25832.501, 197.051*ft)
model.node(120008, 30225.686, 25826.084, 196.808*ft)
model.node(130001, 31043.962, 26673.037, 192.337*ft)
model.node(130002, 31338.594, 26707.787, 191.745*ft)
model.node(130003, 31633.226, 26742.538, 191.188*ft)
model.node(130004, 31927.858, 26777.289, 190.665*ft)
model.node(130005, 30638.228, 25813.678, 196.460*ft)
model.node(130006, 30837.398, 25807.689, 196.384*ft)
model.node(130007, 31036.569, 25801.699, 196.353*ft)
model.node(130008, 31235.740, 25795.709, 196.368*ft)
model.node(140001, 32552.005, 26850.906, 189.683*ft)
model.node(140002, 32881.519, 26889.773, 189.240*ft)
model.node(140003, 33211.033, 26928.640, 188.868*ft)
model.node(140004, 33540.547, 26967.506, 188.566*ft)
model.node(140005, 31783.829, 25779.163, 196.639*ft)
model.node(140006, 32132.789, 25768.658, 196.984*ft)
model.node(140007, 32481.748, 25758.154, 197.462*ft)
model.node(140008, 32830.708, 25747.650, 198.074*ft)
model.node(1010, 10779.297, 31606.976, 194.965*ft)
model.node(1020, 10935.542, 31809.096, 193.626*ft)
model.node(1030, 10623.052, 31404.856, 196.183*ft)
model.node(15010, 33870.061, 27006.373, 188.346*ft)
model.node(15020, 34094.051, 27297.117, 188.604*ft)
model.node(15030, 33646.070, 26715.629, 188.089*ft)
model.node(15040, 33179.709, 25737.199, 198.822*ft)
model.node(15050, 33257.634, 25838.348, 198.650*ft)
model.node(15060, 33101.704, 25635.948, 198.994*ft)

