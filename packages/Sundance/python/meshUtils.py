#@HEADER@
#@HEADER@

############################################################################
#
# This module is a collection of utilities for converting between mesh
# formats, partitioning meshes, etc.
#
############################################################################

from sets import Set
import posix
import string


#-----------------------------------------------------------------------
#
# runChaco() makes a system call to run Chaco for the graph file
# whose name is given as an argument. The number of processors
# is also given as an input argument
#
#-----------------------------------------------------------------------

def runChaco(graphFile, nProcs) :
    paramsFile = file('User_Params', 'w')
    paramsFile.write('OUTPUT_ASSIGN=true\n')
    paramsFile.write('PROMPT=false\n')
    paramsFile.write('ARCHITECTURE=1\n')
    paramsFile.write('REFINE_PARTITION=4\n')
    paramsFile.write('INTERNAL_VERTICES=true\n')
    paramsFile.write('MATCH_TYPE=4\n')
    paramsFile.write('COARSE_NLEVEL_KL=1\n')
    paramsFile.write('CUT_TO_HOP_COST=1.0\n')
    paramsFile.flush()
    inputFile = file('chacoInput', 'w')
    inputFile.write('%s.graph\n' % graphFile)
    inputFile.write('%s.assign\n' % graphFile)
    inputFile.write('1\n')
    inputFile.write('400\n')
    inputFile.write('%d\n' % nProcs)
    inputFile.write('1\n')
    inputFile.write('n\n')
    inputFile.flush()
    inputFile.close()
    posix.system('chaco < chacoInput')
    
    

#-----------------------------------------------------------------------
#
# makeChacoGraphFile() reads a Triangle element file and works out the
# connectivity between elements, writing a neighbor list for each element
# into a Chaco graph file.
#
#-----------------------------------------------------------------------

def makeChacoGraphFile(filename) : 
    f = file(filename + '.ele')
    nodeToEleMap = {}
    elemVerts = []
    # read header 
    while 1 :
        line = f.readline()
        if line[0]=='#': continue
        header = line.split()
        nElems = int(header[0])
        d = int(header[1])-1
        break
    # read lines, building elements and the element-to-node map
    while 1:
        line = f.readline()
        if not line : break
        if line[0]=='#': continue
        toks = line.split()
        ele = int(toks[0])
        verts = Set()
        for i in range(d+1) :
            node = int(toks[i+1])
            verts.add(node)
            if nodeToEleMap.has_key(node) :
                nodeToEleMap[node].add(ele)
            else :
                nodeToEleMap[node] = Set()
                nodeToEleMap[node].add(ele)
        elemVerts.append(verts)

    # For each node, assign one of the adjoining elements as its "owner."
    # The node will later be assigned to the same processer as the owner.
    # The choice of owner is arbitrary; here, we simply choose the
    # adjoining element having the largest index.
    #
    # We write the ownership information to a file, with the format:
    # line 1: <num nodes>
    # line 2: <node 1 number> <node 1 owner>
    # etc.
    nodeOwnerFile = file(filename + '.owner', 'w')
    nodeOwnerFile.write('%d\n' % len(nodeToEleMap.keys()))
    for node in nodeToEleMap.keys() :
        owner = max(nodeToEleMap[node])
        nodeOwnerFile.write('%d %d\n' % (node, owner))


    
    
    # determine lists of neighbors for each element
    neighbors = []
    nEdges = 0
    for i in range(nElems) :
        allNeighbors = Set()
        for v in elemVerts[i] :
            allNeighbors = Set.union(allNeighbors, nodeToEleMap[v])
        # get rid of self-references
        allNeighbors.discard(i)
        fullNeighbors = []
        for j in allNeighbors :
            numCommonNodes = Set.intersection(elemVerts[i], elemVerts[j])
            if len(numCommonNodes) == d :
                fullNeighbors.append(j)
                
        nEdges = nEdges + len(fullNeighbors)
        neighbors.append(fullNeighbors)

    nEdges = nEdges/2

    graphFile = file(filename + '.graph', 'w')
    graphFile.write('%d %d\n' % (nElems, nEdges))

    for i in range(nElems) :
        line = ''
        for j in neighbors[i] :
            line = line +  '%d ' % (j+1)
        graphFile.write(line + '\n');
    graphFile.flush()

    return (elemVerts, nodeToEleMap)


#-----------------------------------------------------------------------
#
# readNodeFile() reads a Triangle node file. Nodes are not used
# in partitioning, but we have to write node information for the partitioned
# mesh.
#
#-----------------------------------------------------------------------

def readNodeFile(filename) :
    f = file(filename + '.node')
    nodeLines = []
    # read header 
    while 1 :
        line = f.readline()
        if line[0]=='#': continue
        headerline = line
        header = line.split()
        nNodes = int(header[0])
        d = int(header[1])
        nAttr = int(header[2])
        nMark = int(header[3])
        break
    # read lines, building elements and the element-to-node map
    while 1:
        line = f.readline()
        if not line : break
        if line[0]=='#': continue
        data = string.join(line.split()[1:d+1])
        nodeLines.append(data)
        print 'data: ', data

    return (d, nAttr, nMark, nodeLines)



#-----------------------------------------------------------------------
#
# readAssignments() reads processor assignments from a Chaco output file
#
#-----------------------------------------------------------------------

def readElemAssignments(filename) :
    assignments = []
    f = file(filename + '.assign')

    # read lines in the assignment file
    while 1:
        line = f.readline()
        if not line : break
        if line[0]=='#': continue
        assignments.append(int(line))

    return assignments

#-----------------------------------------------------------------------
#
# remap entity numberings so that each processor gets a contiguous chunk of
# entity numbers
#
#-----------------------------------------------------------------------

def remapEntities(assignments, nProc) :

    procBuckets = []

    for p in range(nProc) :
        procBuckets.append([])

    k = 0
    for a in assignments :
        procBuckets[a].append(k)
        k = k + 1

    g = 0
    entityMap = range(len(assignments))
    for p in range(nProc) :
        for i in procBuckets[p] :
            entityMap[i] = g
            g = g + 1

    return entityMap



#-----------------------------------------------------------------------
#
# getNodeAssignments() works out node assignments given element assignments
#
#-----------------------------------------------------------------------

def getNodeAssignments(nProc, elemAssignments,
                       nodeToElemMap) :
    nodeAssignments = []
    nodeOwners = []
    nodesPerProc = range(nProc)
    for node in nodeToElemMap.keys() :
        owner = max(nodeToElemMap[node])
        nodeOwners.append(owner)
        nodeAssignments.append(elemAssignments[owner])
        nodesPerProc[elemAssignments[owner]] = nodesPerProc[elemAssignments[owner]]+1
    
    return (nodeAssignments, nodeOwners, nodesPerProc)

#-----------------------------------------------------------------------
#
# distributeNodes() writes the nodes to NProc different node files
#
#-----------------------------------------------------------------------

def distributeNodes(filename, nProc, nodeAssignments,
                    nodeOwners, nodesPerProc, nodeMap, nodeData) :
    files = []
    for p in range(nProc) :
        f = file(filename + '.%d.%d.node' % (nProc,p), 'w')
        f.write('%d\n' % (nodesPerProc[p]))
        files.append(f)

    for n in range(len(nodeAssignments)) :
        p = nodeAssignments[n]
        gid = nodeMap[n]
        x = nodeData[n]
        files[p].write('%d %s\n' % (gid, x))
    


#-----------------------------------------------------------------------
#
# writePartitionFile() translates a Chaco output file to a Triangle .part file.
# Sundance does not use .part files, but the resulting .part file can be
# used by showme to view the partitioning
#
#-----------------------------------------------------------------------

def writePartitionFile(filename, nElems, nProcs) :
    assignments = readElemAssignments(filename)

    partFile = file(filename + '.part', 'w')
    partFile.write('%d %d\n' % (nElems, nProcs))
    for i in range(len(assignments)) :
        partFile.write('%d %d\n' % (i, assignments[i]))

    
       
#-----------------------------------------------------------------------
#
# exo2ncdf() calls ncdump to convert an Exodus file to NCDF.
#
#-----------------------------------------------------------------------

def exo2ncdf(filename) :
    cmd = 'ncdump %s.exo > %s.ncdf' % (filename, filename)
    posix.system(cmd)

#-----------------------------------------------------------------------
#
# ncdf2tri() calls ncdump to convert a NCDF file to Triangle format
#
#-----------------------------------------------------------------------

def ncdf2tri(filename) :
    cmd = './Exo2Triangle.exe --i=%s.ncdf --o=%s' % (filename, filename)
    posix.system(cmd)

#-----------------------------------------------------------------------
#
# partitionExo() partitions an Exodus file, writing the results in
# modified Triangle format
#
#-----------------------------------------------------------------------

def partitionExo(filename, nProc) :
    exo2ncdf(filename)
    ncdf2tri(filename)
    partitionTri(filename, nProc)


#-----------------------------------------------------------------------
#
# partitionExo() partitions an Exodus file, writing the results in
# modified Triangle format
#
#-----------------------------------------------------------------------

def partitionTri(filename, nProc) :
    (elemVerts, nodeToElemMap) = makeChacoGraphFile(filename)
    runChaco(filename, nProc)
    writePartitionFile(filename, len(elemVerts), nProc)

    

            
        
        
            

