/**
 */
package outfox.MAXENT_train;

import outfox.MAXENT_train.Data;
import odis.conf.ConfigUtils;
import odis.mapred.ext.IMergeMapper;
import odis.serialize.lib.MD5Writable;
import odis.serialize.comparator.ByteBinaryComparator;
import odis.mapred.lib.IdentityMapper;
import odis.mapred.lib.IdentityReducer;
import odis.mapred.lib.ReuseWalker;
import java.util.logging.Level;
import odis.serialize.lib.LongWritable;
import odis.serialize.lib.DoubleWritable;
import odis.mapred.IWritablePairWalker;
import odis.serialize.IWritable;
import odis.mapred.lib.SeqFileHashPartitioner;
import java.io.IOException;
import odis.serialize.IWritableComparable;
import java.text.ParseException;
import toolbox.misc.MD5;
import java.util.Properties;
import java.util.Random;
import java.util.logging.Logger;
import odis.tools.ToolContext;
import odis.mapred.lib.SkipBadRecordSeqFileInputFormat;
import odis.serialize.comparator.ByteBinaryComparator;
import java.util.*;

import odis.cowork.CounterMap.Counter;
import odis.app.job.AbstractCoWorkToolWithArg;
import odis.app.serialize.AbstractBeanWritable;
import odis.cowork.JobDef;
import odis.cowork.JobResult;
import odis.cowork.TaskRunnable;
import odis.io.FSDataInputStream;
import odis.io.FileInfo;
import odis.io.FileSystem;
import odis.io.Path;
import odis.mapred.AbstractMapper;
import odis.mapred.AbstractReducer;
import odis.mapred.ICollector;
import odis.mapred.IWritablePairWalker;
import odis.mapred.MapReduceJobDef;
import odis.mapred.lib.GenericFileOutputFormat;
import odis.mapred.lib.ReuseWalker;
import odis.mapred.lib.StringMd5Partitioner;
import odis.serialize.lib.StringWritable;
import odis.serialize.lib.TimeWritable;
import odis.serialize.lib.UTF8Writable;
import odis.tools.MapReduceHelper;
import toolbox.misc.LogFormatter;
import toolbox.misc.cli.Options;
import toolbox.wordsegment.CSegmentor;
import toolbox.wordsegment.TextTermInfo;
import toolbox.wordsegment.termstat.TermStatReader;
import odis.app.job.AbstractCoWorkToolWithArg;
import odis.cowork.JobResult;
import odis.io.Path;
import odis.mapred.AbstractMapper;
import odis.mapred.AbstractReducer;
import odis.mapred.ICollector;
import odis.mapred.IWritablePairWalker;
import odis.mapred.MapReduceJobDef;
import odis.mapred.lib.ReuseWalker;
import odis.serialize.comparator.IntBinaryComparator;
import odis.serialize.lib.IntWritable;
import odis.tools.MapReduceHelper;
import odis.serialize.lib.Url;
import odis.serialize.toolkit.AbstractMultiDynamicWritable;
import odis.serialize.toolkit.AbstractOpObject;
import odis.serialize.toolkit.AbstractWritableList;
import odis.serialize.toolkit.AbstractDynamicWritable.AbstractClassIdMap;
import odis.serialize.toolkit.AbstractDynamicWritable.IClassIdMap;
import odis.tools.AbstractCoWorkTool;
import odis.tools.MapReduceHelper;
import odis.tools.ToolContext;
import outfox.MAXENT_train.Data;

public class trainMAXENT extends AbstractCoWorkToolWithArg {
  static final int shift=8;
  static final int mask=(1<<shift)-1;
  private static long total_sample=0;
  private static int max_sample_per_feature=0;
  private double equation_precision=1e-9;
    private String inDir;
    private String outDir;
    private String tmpDir;
    private String f2sDir;
    private String s2fDir;
  private int itr_times;
  private double delta_upper;
  static final private long error_factor=1000000000;
  private double error_upper;
  private double sigma_upper,sigma_denom,sigma_adder;

  private int reducer_num;
    protected void prepareOptions(Options options) {
        options.withOption("i", "input dir",
                "request-log [TimeWritable, UTF8Writbale]");
      //  options.withOption("o", "output dir",
        //        "queries and their frequencies [StringWritable, IntWritable]")
          //      .hasDefault();
    options.withOption("rn", "reducer number","reducer number");
    options.withOption("f2s", "f2s output dir","");
    options.withOption("s2f", "s2f output dir","");
    options.withOption("start", "start phrase","");
    options.withOption("itr", "reducer number","");
    //options.withOption("error", "Equation error upper","");
    }
  int start;
    public boolean processOptions(Options options) throws Exception {
    itr_times=Integer.parseInt(options.getStringOpt("itr","default"));
    //error_upper=Double.parseDouble(options.getStringOpt("error","default"));

        inDir = options.getStringOpt("i","default");
        s2fDir = options.getStringOpt("s2f", "default");
        f2sDir = options.getStringOpt("f2s", "default");
    reducer_num=Integer.parseInt(options.getStringOpt("rn","default"));
    start=Integer.parseInt(options.getStringOpt("start","default"));
    if(start>2)Data.throw_exception();
    if(inDir.compareTo(s2fDir)==0||inDir.compareTo(f2sDir)==0||f2sDir.compareTo(s2fDir)==0)
    {
      System.out.println("The same inDir and outDir");
      return false;
    }
        return true;
    }

    @Override
    public String comment() {
        return "MAXENT traning";
    }
  private Counter itr_counter;
    @Override
    public boolean exec(int nWorker) throws Exception {

        Path input = new Path(inDir);
        Path s2f_output = new Path(s2fDir);
        Path f2s_output = new Path(f2sDir);
    assert(reducer_num>0);
    String conf_file = ToolContext.getAppHome() + "/conf/NLP_classifier.conf";
    HashMap<String,String> config=Data.readConfig(conf_file);
    max_sample_per_feature=Integer.parseInt((String)config.get("max_sample_per_feature"));
    total_sample=Long.parseLong((String)config.get("total_sample"));
    equation_precision=Double.parseDouble((String)config.get("equation_precision"));
    error_upper=Double.parseDouble((String)config.get("error_upper"));
    delta_upper=Double.parseDouble((String)config.get("delta_upper"));
    sigma_upper=Double.parseDouble((String)config.get("sigma_upper"));
    sigma_denom=Double.parseDouble((String)config.get("sigma_denom"));
    sigma_adder=Double.parseDouble((String)config.get("sigma_adder"));
    System.out.println("reducer_num="+reducer_num);
    System.out.println("max_sample_per_fea="+max_sample_per_feature);
    System.out.println("total_sample="+total_sample);
    System.out.println("equation_precision="+equation_precision);
    System.out.println("start="+start);
    System.out.println("itr_times="+itr_times);
    System.out.println("error upper="+error_upper);
    System.out.println("delta upper="+delta_upper);
    if(start<=0)
    {
      long start_time=new Date().getTime()/1000;
      System.out.println("This is phrase #0");
      FileSystem fs = context.getFileSystem();
          MapReduceJobDef job = context.createMapReduceJob(this.getToolName(),nWorker);
          MapReduceHelper helper = new MapReduceHelper(context, job);
          helper.addReadInputDir(input,SkipBadRecordSeqFileInputFormat.class);
      job.setCheckReduceProgress(false);
          job.setMapper(Mapper.class);
          job.setMapNumber(nWorker * mParallel);
          job.setMergeKeyValClass(Data.feature_t.class, Data.sample_t.class);
          job.setPartitionerClass(Data.feature_partitioner.class);
          job.setReducer(Reducer.class);
          job.setWalkerClass(ReuseWalker.class);
      job.setReduceNumber(reducer_num);
          helper.addDirectOutputDir(0, s2f_output, Data.sample_t.class,Data.feature_t.class, null);
      job.getConfig().setLong("total_sample",total_sample);
      job.getConfig().setLong("max_sample_per_feature",max_sample_per_feature);
      job.getConfig().setDouble("equation_precision",equation_precision);
          JobResult result = helper.runJob(context.getCoWork());
          if (!result.isSuccess()) return false;
      if (result.isSuccess()) {
              helper.printMessages(out, result.getMsg());
              helper.printCounters(out, result.getCounters());
          }
      System.out.println("Phrase #0 took "+(new Date().getTime()/1000-start_time)+" seconds");
        }
    int max_sample_per_fea=max_sample_per_feature;
    System.out.println("reducer_num="+reducer_num);
    for(int i=0;i<itr_times;i++)
    {
      F2S_Reducer.itr_num=i+1;
      System.out.println("This is iteration #"+i);
      if(start<=1)
      {
        long start_time=new Date().getTime()/1000;
        System.out.println("This is phrase #1");
        System.out.println("Start calculating sample probability from s2f file and will generate f2s file");
        System.out.println("max_sample_per_fea="+max_sample_per_feature);
        FileSystem fs = context.getFileSystem();
            MapReduceJobDef job = context.createMapReduceJob(this.getToolName(),nWorker);
        job.getConfig().setLong("total_sample",total_sample);
        job.getConfig().setLong("max_sample_per_feature",max_sample_per_fea);
        job.getConfig().setDouble("equation_precision",equation_precision);
            MapReduceHelper helper = new MapReduceHelper(context, job);
            helper.addReadInputDir(s2f_output,SkipBadRecordSeqFileInputFormat.class);
        job.setCheckReduceProgress(false);
            job.setMapper(IdentityMapper.class);
            job.setMapNumber(nWorker * mParallel);
            job.setMergeKeyValClass(Data.sample_t.class, Data.feature_t.class);
            job.setPartitionerClass(Data.sample_partitioner.class);
            job.setReducer(S2F_Reducer.class);
            job.setWalkerClass(ReuseWalker.class);
        job.setReduceNumber(reducer_num);
            helper.addDirectOutputDir(0, f2s_output, Data.feature_t.class,Data.sample_t.class, null);
            JobResult result = helper.runJob(context.getCoWork());
            if (!result.isSuccess()) return false;
        if (result.isSuccess()) {
                helper.printMessages(out, result.getMsg());
                helper.printCounters(out, result.getCounters());
            }
        System.out.println("Finished calculating sample probability from s2f file.");
        System.out.println("Phrase #1 took "+(new Date().getTime()/1000-start_time)+" seconds");
      }
      boolean end=false;
      if(start<=2)
      {
        long start_time=new Date().getTime()/1000;
        System.out.println("This is phrase #2");
        System.out.println("Start updating parameters from f2s file and will generate s2f file.");
        System.out.println("max_sample_per_fea="+max_sample_per_fea);
        FileSystem fs = context.getFileSystem();  
            MapReduceJobDef job = context.createMapReduceJob(this.getToolName(),nWorker);
        job.getConfig().setLong("total_sample",total_sample);
        job.getConfig().setLong("max_sample_per_feature",max_sample_per_fea);
        job.getConfig().setDouble("equation_precision",equation_precision);
        job.getConfig().setDouble("sigma_upper",sigma_upper);
        job.getConfig().setDouble("sigma_denom",sigma_denom);
        job.getConfig().setDouble("sigma_adder",sigma_adder);
            MapReduceHelper helper = new MapReduceHelper(context, job);
            helper.addReadInputDir(f2s_output,SkipBadRecordSeqFileInputFormat.class);
        job.setCheckReduceProgress(false);
            job.setMapper(IdentityMapper.class);
            job.setMapNumber(nWorker * mParallel);
            job.setMergeKeyValClass(Data.feature_t.class, Data.sample_t.class);
            job.setPartitionerClass(Data.feature_partitioner.class);
            job.setReducer(F2S_Reducer.class);
            job.setWalkerClass(ReuseWalker.class);
        job.setReduceNumber(reducer_num);
            helper.addDirectOutputDir(0, s2f_output, Data.sample_t.class,Data.feature_t.class, null);
            JobResult result = helper.runJob(context.getCoWork());
            if (!result.isSuccess()) return false;
        if (result.isSuccess()) {
                helper.printMessages(out, result.getMsg());
                helper.printCounters(out, result.getCounters());
            }
        long delta_sum= result.getCounters()[1].get("delta_sum").get();
        long Newton_times= result.getCounters()[1].get("Newton_times").get();
        System.out.println("Newton_times="+Newton_times+" at Iteration#"+(i+1));
        long feature_counter= result.getCounters()[1].get("feature_counter").get();
        System.out.println("feature_counter="+feature_counter+" at Iteration#"+(i+1));
        double max_equation_error=0.0;
        for(int idx=0;idx<reducer_num;idx++)
        {
          long error=result.getCounters()[1].get("equation_error_"+idx).get();
          max_equation_error=Math.max(1.0*error/error_factor,max_equation_error);
        }
        System.out.println("max equation error="+max_equation_error);
        double average_delta=1.0*delta_sum/1000/feature_counter/2;
        System.out.println("Average delta="+average_delta);
        long error_sum=result.getCounters()[1].get("equation_error_sum").get();
        double average_error=1.0*error_sum/error_factor/feature_counter/2;
        System.out.println("Average error="+average_error);
        end=average_error<error_upper||average_delta<delta_upper;
        System.out.println("Phrase #2 took "+(new Date().getTime()/1000-start_time)+" seconds");
      }
      System.out.println("Finished updating parameters from f2s file.");
      System.out.println("Finished iteration #"+(i+1));
      start=-1;
      if(end)
      {
        System.out.println("Succeeded to reach ending condition with delta_upper="+delta_upper);
        break;
      }
        }
        return true;
    }
  
  public static class S2F_Reducer extends AbstractReducer<Data.sample_t, Data.feature_t> {
    Data.sample_t sam;
    Data.feature_t fea;
    Data.feature_list flist=new Data.feature_list();
    Counter sample_counter,line_counter;
        public void configure(JobDef job, TaskRunnable task) {
      sam=new Data.sample_t();
      fea=new Data.feature_t();
      sample_counter=task.getCounter("sample_counter");
      line_counter=task.getCounter("line_counter");
        }
        public void reduce(Data.sample_t sample,IWritablePairWalker<Data.sample_t, Data.feature_t> values,
            ICollector collector) 
    {
      flist.clear();
      double pos=0,neg=0;
      sample_counter.inc(sample.portion.get());
      line_counter.inc();
      while(values.moreValue())
      {
        Data.feature_t f=values.getValue();
        pos+=f.lam_pos.get();
        neg+=f.lam_neg.get();
        flist.add(new Data.feature_t(f));
      }
      sample.positive_prob.set(1.0/(1.0+Math.exp(neg-pos)));
      if((!(sample.positive_prob.get()>0.0)) && (!(sample.positive_prob.get()<1.0)))
      {
        System.out.println(neg+" "+pos);
        Data.throw_exception();
      }
      for(int i=0;i<flist.size();i++)
        collector.collect(flist.get(i),sample);
        }
        public void reduceEnd(ICollector collector) {}
    }
  public static class F2S_Reducer extends AbstractReducer<Data.feature_t, Data.sample_t> 
  {
    static public int itr_num;
    static boolean update_para;
    Counter delta_sum,Newton_times,reducer_num_counter;
    Data.feature_t feature;
    Data.sample_t sample;
    double positive_prob[];
    int hit_portion[];
    int sid[];
    private Counter itr_counter,binary_counter;
    Counter feature_counter;
    int max_sample_per_fea;
    long total_sample;
    double equation_precision;
    private Counter equation_error_counter;
    private Counter equation_error_sum_counter;
    double max_equation_error;
    double sigma_upper,sigma_denom,sigma_adder;
        public void configure(JobDef job, TaskRunnable task) 
    {
      max_sample_per_fea=(int)job.getConfig().getLong("max_sample_per_feature");
      equation_precision=job.getConfig().getDouble("equation_precision");
      sigma_upper=job.getConfig().getDouble("sigma_upper");
      sigma_denom=job.getConfig().getDouble("sigma_denom");
      sigma_adder=job.getConfig().getDouble("sigma_adder");
      total_sample=job.getConfig().getLong("total_sample");
      delta_sum=task.getCounter("delta_sum");
      Newton_times=task.getCounter("Newton_times");
      reducer_num_counter=task.getCounter("reducer_num_counter");
      feature_counter=task.getCounter("feature_counter");
      binary_counter=task.getCounter("binary_counter");
      feature=new Data.feature_t();
      sample=new Data.sample_t();
      positive_prob=new double[max_sample_per_fea];
      hit_portion=new int[max_sample_per_fea];
      sid=new int[max_sample_per_fea];
      reducer_num_counter.inc();
      long partIdx=task.getPartIdx();
      equation_error_counter=task.getCounter("equation_error_"+partIdx);
      equation_error_sum_counter=task.getCounter("equation_error_sum");
      max_equation_error=0.0;
      System.out.println("max_sample_per_fea="+max_sample_per_fea);
      System.out.println("equation_precision="+equation_precision);
      System.out.println("sigma_upper="+sigma_upper);
      System.out.println("sigma_denom="+sigma_denom);
      System.out.println("sigma_adder="+sigma_adder);
        }
    private double solve_equation(double aa[],int bb[],double target,int n,double extra_linear_cof) 
    {
      if(n<=0)Data.throw_exception();
      //直接以初值0作Newdon迭代
      double x=0.0,y;
      int times=0;
      for(;;)
      {
        double dy=0.0;
        y=0.0;
        for(int i=0;i<n;i++)
        {
          double add=aa[i]*Math.exp(x*(bb[i]&mask));
          if(Data.isNaN(add))
          {
            System.out.println(x+" "+(bb[i]&mask));
            Data.checkNaN(add);
          }
          y+=add;
          dy+=(bb[i]&mask)*add;
          //System.out.println("aa[i]="+aa[i]+" bb[i]="+(bb[i]&mask)+" x="+x+" add="+add+" dy_add="+((bb[i]&mask)*add));
        }
        y+=extra_linear_cof*x-target;
        dy+=extra_linear_cof;
        x-=y/dy;
        Newton_times.inc();
        times++;
        if(Math.abs(y)/target<equation_precision)break;
        //System.out.println("x="+x+" y="+y+" dy="+dy);
      }
//      LOG.log(Level.WARNING, "Iteration #"+itr_num+" times="+times+" y="+y+" target="+target);
      System.out.println("target="+target+" x="+x+" y="+y+" precision="+Math.abs(y)/target);
      if((!(x<1.0))&&(!(x>0.0)))
      {
        System.out.println("NaN x="+x+" times="+times);
        System.out.println("target="+target+" x="+x+" y="+y);
        for(int i=0;i<n;i++)
        {
          System.out.print(aa[i]+","+(bb[i]&mask)+" ");
        }
        System.out.println("");
        Data.throw_exception();
      }
      return x;
    }
        public void reduce(Data.feature_t feature,IWritablePairWalker<Data.feature_t, Data.sample_t> values,
              ICollector collector) 
    {
      int n=0;
      feature_counter.inc();
      while(values.moreValue())
      {
        Data.sample_t sam=values.getValue();
        if(n>=hit_portion.length)Data.throw_exception();
        positive_prob[n]=(double)sam.positive_prob.get();
        sid[n]=sam.sid.get();
        if(sam.portion.get()>(1<<(32-shift))||sam.hit.get()>mask)Data.throw_exception();
        hit_portion[n++]=(sam.portion.get()<<shift)^sam.hit.get();
      }
      final double sigma=sigma_upper/(1+Math.exp(-Math.sqrt(feature.coverage.get())/sigma_denom+sigma_adder));
      System.out.println("feature #"+feature.fid.toString()+" sigma="+sigma);
      double pos_error=0.0,neg_error=0.0;
      for(int i=0;i<n;i++)
      {
        positive_prob[i]=(double)(1-positive_prob[i])*(hit_portion[i]>>shift)/total_sample;
        neg_error+=positive_prob[i];
      }
      //加上正规化项
      double dneg=solve_equation(positive_prob,hit_portion,
          feature.exp_neg.get()-feature.lam_neg.get()/sigma/sigma,n,1.0/sigma/sigma);
      for(int i=0;i<n;i++)
      {
        positive_prob[i]=(double)(1.0*(hit_portion[i]>>shift)/total_sample-positive_prob[i]);
        pos_error+=positive_prob[i];
      }
      //加上正规化项
      double dpos=solve_equation(positive_prob,hit_portion,
          feature.exp_pos.get()-feature.lam_pos.get()/sigma/sigma,n,1.0/sigma/sigma);
      System.out.println("feature #"+feature.fid.toString()+": "+dpos+","+dneg);
      if(Math.abs(dneg)>1e20||Math.abs(dpos)>1e20)
      {
        System.out.println(dpos+","+dneg);
        Data.throw_exception();
      }
      long add=(long)((Math.abs(dpos)+Math.abs(dneg))*1000);
      delta_sum.inc(add);
      feature.lam_pos.set(feature.lam_pos.get()+dpos);
      feature.lam_neg.set(feature.lam_neg.get()+dneg);
      for(int i=0;i<n;i++)
      {
        sample.sid.set(sid[i]);
        sample.portion.set(hit_portion[i]>>shift);
        sample.positive_prob.set(positive_prob[i]*total_sample/(hit_portion[i]>>shift));
        sample.hit.set(hit_portion[i]&mask);
        collector.collect(sample,feature);
      }
      pos_error=Math.abs(pos_error-feature.exp_pos.get()+feature.lam_pos.get()/sigma/sigma)
        /feature.exp_pos.get();
      neg_error=Math.abs(neg_error-feature.exp_neg.get()+feature.lam_pos.get()/sigma/sigma)
        /feature.exp_neg.get();
      max_equation_error=Math.max(pos_error,max_equation_error);
      max_equation_error=Math.max(neg_error,max_equation_error);
      equation_error_sum_counter.inc((long)((pos_error+neg_error)*error_factor));
        }
        public void reduceEnd(ICollector collector) 
    {
      equation_error_counter.inc((long)(error_factor*max_equation_error));
    }
    }

  
  
  public static class Mapper extends
            AbstractMapper<Data.sample_t, Data.feature_t> {

        public void configure(JobDef job, TaskRunnable task) {
        }

    public void map(Data.sample_t key, Data.feature_t feature,ICollector collector) 
    {
      collector.collect(feature,key);
    }
    }

    public static class Reducer extends AbstractReducer<Data.feature_t, Data.sample_t> {

    private TaskRunnable p_task;
    int sid[];
    int hit_portion[];
    double positive_prob[];
    Counter feature_counter,sample_counter,line_counter;
    int max_sample_per_feature,max_sample_per_fea;
    long total_sample;
        public void configure(JobDef job, TaskRunnable task) {
      max_sample_per_fea=max_sample_per_feature=
        (int)(job.getConfig().getLong("max_sample_per_feature"));
      total_sample=(job.getConfig().getLong("total_sample"));
      if(total_sample<=0)Data.throw_exception();
      feature_counter=task.getCounter("feature_counter");
      sample_counter=task.getCounter("sample_counter");
      line_counter=task.getCounter("line_counter");
      sid=new int[max_sample_per_feature];
      hit_portion=new int[max_sample_per_feature];
      positive_prob=new double[max_sample_per_feature];
      System.out.println("max_sample_per_fea="+max_sample_per_fea);
      System.out.println("total_sample="+total_sample);
        }
        public void reduce(Data.feature_t feature,IWritablePairWalker<Data.feature_t, Data.sample_t> samples,
            ICollector collector) 
    {
      double pos=0.0,neg=0.0;
      int top=0;
      feature_counter.inc();
      while(samples.moreValue())
      {
        Data.sample_t sam=samples.getValue();
        if(top>=max_sample_per_fea)Data.throw_exception();
        if(sam.portion.get()>=((1<<(32-shift)))||sam.hit.get()>mask)Data.throw_exception();
        sid[top]=(int)sam.sid.get();
        hit_portion[top]=((int)sam.hit.get())^((int)sam.portion.get()<<shift);
        top++;
        //这里的positive_prob并不是概率，而是平滑后的正例个数
        pos+=sam.positive_prob.get();
        neg+=sam.portion.get()-sam.positive_prob.get();
        if(sam.positive_prob.get()<0.0||sam.portion.get()+1e-10<sam.positive_prob.get())
        {
          System.out.println(sam.toString());
          Data.throw_exception();
        }
      }
      pos/=total_sample;
      neg/=total_sample;
      feature.exp_pos.set(pos);
      feature.exp_neg.set(neg);
      Data.checkNaN(pos);
      Data.checkNaN(neg);
      if(pos<1e-30||neg<1e-30)
      {
        System.out.println("pos="+pos+" neg="+neg);
        Data.throw_exception();
      }
      feature.lam_pos.set(0.0);
      feature.lam_neg.set(0.0);
      for(int i=0;i<top;i++)
      {  
        Data.sample_t sample=new Data.sample_t();
        sample.sid.set(sid[i]);
        sample.hit.set(hit_portion[i]&mask);
        sample.positive_prob.set(0.5);
        sample.portion.set(hit_portion[i]>>shift);
        collector.collect(sample,feature);
      }
        }
        public void reduceEnd(ICollector collector) {}
    }
  public static void main(String args[])
  {
    System.out.println(Math.exp(-100000));
  }
}
