# ZeroLength5.tcl
# CE 221 Spring 1999 Midterm Problem #1
#
# Written: MHS
# Date: Jan 2000
#
#  |A        B
#  |@--------@------------
#  |    4          6      ^

# Define the model builder
model BasicBuilder -ndm 2 -ndf 3

# Define nodes
node 1   0.0   0.0
node 2   0.0   0.0
node 3   4.0   0.0
node 4   4.0   0.0
node 5  10.0   0.0

# Define single point constraints
fix 1  1 1 1
fix 5  1 1 0

# Define moment-rotation relationship for spring A
uniaxialMaterial ElasticPP 1 10 0.8

# Define moment-rotation relationship for spring B
uniaxialMaterial ElasticPP 3  6 1.0
uniaxialMaterial Elastic   4  4
uniaxialMaterial Parallel  2  3 4

# Geometric transformation
geomTransf Linear 1

# Define beam elements
element elasticBeamColumn 3 2 3 100 1000 1000 1
element elasticBeamColumn 4 4 5 100 1000 1000 1

source Test.rotSpring2D.tcl

#           eleID nodeR nodeC matID
rotSpring2D   1     1     2     1
rotSpring2D   2     3     4     2

pattern Plain 1 Linear {
	load  3  0.0  10.0  0.0
}

integrator LoadControl 1 1 1 1
test NormDispIncr 1.0e-8 10 1
algorithm KrylovNewton -maxDim 3
numberer Plain
constraints Penalty 1.0e12 1.0e12
system UmfPack
analysis Static

analyze 1

print algorithm

wipe
