#@HEADER@
#@HEADER@


############################################################################
#
# Python mesh class
#
############################################################################

from sets import Set

class Mesh :
    def __init__(self) :
        self.pts_ = []
        self.elemVerts_ = []
        self.vertToElemMap_ = {}
        self.dim_ = 0

    def setDimension(self, d) :
        self.dim_ = d
        
    def numPts(self) :
        return len(self.pts_)

    def numElems(self) :
        return len(self.elemVerts_)

    def addPoint(self, pt) :
        self.pts_.append(pt)

    def addElem(self, elemVerts) :
        lid = len(self.elemVerts_)
        self.elemVerts_.append(elemVerts)

        for v in elemVerts :
            if v not in self.vertToElemMap_ :
                self.vertToElemMap_[v] = Set()
            self.vertToElemMap_[v].add(lid)

    def findNeighbors(self) :
        neighbors = []
        nEdges = 0
        for i in range(self.numElems()) :
            allNeighbors = Set()
            for v in self.elemVerts_[i] :
                allNeighbors = Set.union(allNeighbors, self.vertToElemMap_[v])
            # get rid of self-references
            allNeighbors.discard(i)
            fullNeighbors = []
            for j in allNeighbors :

                numCommonNodes = Set.intersection(self.elemVerts_[i],
                                                  self.elemVerts_[j])
                if len(numCommonNodes) == self.dim_ :
                    fullNeighbors.append(j)
                         
            nEdges = nEdges + len(fullNeighbors)
            neighbors.append(fullNeighbors)

        nEdges = nEdges/2

        return (neighbors, nEdges)

    def writeGraph(self, filename) :
         (neighbors, nEdges) = self.findNeighbors()
         graphFile = file(filename + '.graph', 'w')
         graphFile.write('%d %d\n' % (self.numElems(), nEdges))

         for i in range(self.numElems()) :
             line = ''
             for j in neighbors[i] :
                 line = line +  '%d ' % (j+1) 
             graphFile.write(line + '\n');


    def remapEntities(self, assignments, nProc) :

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

    def getNodeAssignments(self, nProc, elemAssignments) :
        nodeAssignments = []
        nodeOwners = []
        nodesPerProc = range(nProc)
        for node in self.vertToElemMap_.keys() :
            owner = max(self.vertToElemMap_[node])
            nodeOwners.append(owner)
            nodeAssignments.append(elemAssignments[owner])
            nodesPerProc[elemAssignments[owner]] = nodesPerProc[elemAssignments[owner]]+1
    
        return (nodeAssignments, nodeOwners, nodesPerProc)


    def getOffProcData(self, p, elemAssignments, nodeAssignments) :

        # first pass: find off-proc nodes required by on-proc elems
        offProcNodes = Set()
        for e in range(len(self.elemVerts_)) :
            if elemAssignments[e] != p :
                continue
            for v in self.elemVerts_[e] :
                if nodeAssignments[v] == p :
                    continue
                offProcNodes.add(v)

        # second pass: find off-proc elems required by the nodes
        # obtained in the previous step
        offProcElems = Set()
        for v in offProcNodes :
            for e in self.vertToElemMap_[v] :
                if elemAssignments[e] == p :
                    continue
                offProcElems.add(e)

        # third pass: find the additional nodes required by the off-proc
        # elems found in the previous step

        for e in offProcElems :
            for v in self.elemVerts_[e] :
                if nodeAssignments[v] == p :
                    continue
                offProcNodes.add(v)

        return (offProcNodes, offProcElems)
    
            


        
                
            

        
        

    
        
        
